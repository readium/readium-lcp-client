//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#include "LcpService.h"
#include "CryptoLcpNode.h"
#include "LinksLcpNode.h"
#include "UserLcpNode.h"
#include "RightsLcpNode.h"
#include "RootLcpNode.h"
#include "LcpUtils.h"
#include "JsonValueReader.h"
#include "JsonCanonicalizer.h"
#include "EncryptionProfilesManager.h"
#include "CryptoppCryptoProvider.h"
#include "SimpleKeyProvider.h"
#include "public/IStorageProvider.h"
#include "Acquisition.h"
#include "RightsService.h"
#include "LsdProcessor.h"
#include "StatusDocumentProcessing.h"


namespace lcp
{
    /*static*/ std::string LcpService::UnknownProvider = "UnknownProvider";
    /*static*/ std::string LcpService::UnknownUserId = "UnknownUserId";

    LcpService::LcpService(
        const std::string & rootCertificate,
        INetProvider * netProvider,
        IStorageProvider * storageProvider,
        IFileSystemProvider * fileSystemProvider,
        const std::string & defaultCrlUrl
        )
        : m_rootCertificate(rootCertificate)
        , m_netProvider(netProvider)
        , m_storageProvider(storageProvider)
        , m_fileSystemProvider(fileSystemProvider)
        , m_rightsService(new RightsService(m_storageProvider, UnknownUserId))
        , m_jsonReader(new JsonValueReader())
        , m_encryptionProfilesManager(new EncryptionProfilesManager())
        , m_cryptoProvider(new CryptoppCryptoProvider(m_encryptionProfilesManager.get(), m_netProvider, defaultCrlUrl))
#if !DISABLE_LSD
        , m_LicenseStatusDocumentThatStartedProcessing(nullptr)
#endif //!DISABLE_LSD
    {
    }


    Status LcpService::OpenLicense(
            const ePub3::string & publicationPath,
            const std::string & licenseJson,
            ILicense** licensePTR)
    {
        // When no EPUB path is provided, this means the LCPL file is opened directly (client needs "publication" link to acquire / download the EPUB)
        m_publicationPath = publicationPath;

        try
        {
            std::string canonicalJson = this->CalculateCanonicalForm(licenseJson);

            bool foundLicense = this->FindLicense(canonicalJson, licensePTR);
            if (foundLicense) {
                Status res = Status(StatusCode::ErrorCommonSuccess);

                if (!(*licensePTR)->Decrypted()) {
                    // THIS SHOULD NEVER HAPPEN! (because the LCP license is only added to the cache when it is succesfully decrypted)
                    res = this->CheckDecrypted((*licensePTR));
                }

#if !DISABLE_LSD
                res = this->CheckLicenseStatusDocument((*licensePTR));
#endif //!DISABLE_LSD

                return res;
            }

            std::unique_ptr<CryptoLcpNode> cryptoNode(new CryptoLcpNode(m_encryptionProfilesManager.get()));
            std::unique_ptr<LinksLcpNode> linksNode(new LinksLcpNode());
            std::unique_ptr<UserLcpNode> userNode(new UserLcpNode());
            std::unique_ptr<RightsLcpNode> rightsNode(new RightsLcpNode());
            std::unique_ptr<RootLcpNode> rootNode(new RootLcpNode(
                licenseJson,
                canonicalJson,
                cryptoNode.get(),
                linksNode.get(),
                userNode.get(),
                rightsNode.get())
                );

            rootNode->AddChildNode(std::move(cryptoNode));
            rootNode->AddChildNode(std::move(linksNode));
            rootNode->AddChildNode(std::move(userNode));
            rootNode->AddChildNode(std::move(rightsNode));

            auto parentValue = rapidjson::Value(rapidjson::kNullType);
            rootNode->ParseNode(parentValue, m_jsonReader.get());

            Status res = rootNode->VerifyNode(rootNode.get(), this, m_cryptoProvider.get());
            if (!Status::IsSuccess(res)) {
                return res;
            }

            std::unique_lock<std::mutex> locker(m_licensesSync);
            auto insertRes = m_licenses.insert(
                    std::make_pair(std::move(canonicalJson), std::move(rootNode)));
            if (!insertRes.second) {
                return Status(StatusCode::ErrorOpeningDuplicateLicenseInstance,
                              "Two License instances with the same canonical form");
            }
            (*licensePTR) = insertRes.first->second.get();
            locker.unlock();

            Status result = Status(StatusCode::ErrorCommonSuccess);

            result = this->CheckDecrypted((*licensePTR));

#if !DISABLE_LSD
            result = this->CheckLicenseStatusDocument((*licensePTR));
#endif //!DISABLE_LSD

            return result;
        }
        catch (const StatusException & ex)
        {
            return ex.ResultStatus();
        }
    }

    Status LcpService::CheckDecrypted(ILicense* license) {

        Status result = Status(StatusCode::ErrorCommonSuccess);

        if (!license->Decrypted())
        {
            result = this->DecryptLicenseOnOpening(license);
            
            if (result.Code == StatusCode::ErrorDecryptionLicenseEncrypted) {
                //ASSERT (!license->Decrypted())

                // failure to decrypt using stored user key, we need user passphrase input
                result = Status(StatusCode::ErrorCommonSuccess);
                    
                // Execution flow: on the client side of the LcpService::OpenLicense() API (i.e. LCP Content Module), LICENSE->Decrypted() boolean is checked, and if false then ePub3::ContentModuleExceptionDecryptFlow is raised, then captured quietly where ePub3::Container::OpenContainer() is invoked. Meanwhile, the LCP Crendential Handler triggers asynchronous user action (passphrase input), and this is then followed with another attempt from scratch to "open" the license (see this->DecryptLicense(lic, pass)), and if successful then ePub3::Container::OpenContainer() is invoked all over again (total inversion of control, 2 separate phases).

            } else if (Status::IsSuccess(result)) {
                //ASSERT (license->Decrypted())

                result = Status(StatusCode::ErrorCommonSuccess);
            }
        }

        return result;
    }

#if !DISABLE_LSD

    Status LcpService::CheckLicenseStatusDocument(ILicense* license)
    {
        try
        {
            if (license == nullptr)
            {
                throw std::invalid_argument("license pointer is nullptr");
            }

            // if (!license->Decrypted())
            // {
            //     return Status(StatusCode::ErrorDecryptionLicenseEncrypted);
            // }

            // RootLcpNode * rootNode = dynamic_cast<RootLcpNode *>(license);
            // if (rootNode == nullptr)
            // {
            //     throw std::logic_error("Can not cast ILicense to ILcpNode / RootLcpNode");
            // }
            
            ILinks* links = license->Links();

            //bool foundLink = links->GetLink(Status, lsdLink);
            //if (!foundLink) {
            if (!links->Has(StatusDocument))
            {
                return Status(StatusCode::ErrorCommonSuccess); // no LSD, noop
                //return Status(StatusCode::ErrorStatusDocumentNoStatusLink);
            }

            lcp::Link lsdLink;
            links->GetLink(StatusDocument, lsdLink);
            if (lsdLink.type != LsdProcessor::StatusType)
            {
                return Status(StatusCode::ErrorCommonSuccess); // bogus LSD link, noop
                //return Status(StatusCode::ErrorStatusDocumentWrongType);
            }

            if (!lsdLink.href.length())
            {
                return Status(StatusCode::ErrorCommonSuccess); // bogus LSD link, noop
                //return Status(StatusCode::ErrorStatusDocumentInvalidUri);
            }

            if (m_LicenseStatusDocumentThatStartedProcessing != nullptr) {
                // should be equal to license! (but we check anyway)
                if (m_LicenseStatusDocumentThatStartedProcessing == license) {

                    // The LSD was checked at the last round, so now the EPUB is opening without LSD check.
                    m_LicenseStatusDocumentThatStartedProcessing = nullptr;
                    return Status(StatusCode::ErrorCommonSuccess);
                } else {

                    // different license from previous round?!
                    bool breakpoint = true; // TODO
                }
            }

            m_LicenseStatusDocumentThatStartedProcessing = license;
            // There is a link ... async process must start to attempt the HTTP request, LSD parse, license update, etc.
            return Status(StatusCode::LicenseStatusDocumentStartProcessing);
        }
        catch (const StatusException & ex)
        {
            return Status(StatusCode::ErrorCommonSuccess); // any LSD problem, noop
            //return ex.ResultStatus();
        }
    }

#endif //!DISABLE_LSD

    Status LcpService::DecryptLicense(ILicense * license, const std::string & userPassphrase)
    {
        try
        {
            if (license == nullptr)
            {
                throw std::invalid_argument("license is nullptr");
            }

            KeyType userKey;
            Status res = m_cryptoProvider->DecryptUserKey(userPassphrase, license, userKey);
            if (!Status::IsSuccess(res))
                return res;

            res = this->DecryptLicenseByUserKey(license, userKey);
            if (!Status::IsSuccess(res))
                return res;

            return this->AddDecryptedUserKey(license, userKey);
        }
        catch (const StatusException & ex)
        {
            return ex.ResultStatus();
        }
    }

    Status LcpService::DecryptLicenseByUserKey(ILicense * license, const KeyType & userKey)
    {
        KeyType contentKey;
        Status res = m_cryptoProvider->DecryptContentKey(userKey, license, contentKey);
        if (!Status::IsSuccess(res))
            return res;

        RootLcpNode * rootNode = dynamic_cast<RootLcpNode *>(license);
        if (rootNode == nullptr)
        {
            throw std::logic_error("Can not cast ILicense to ILcpNode");
        }

        rootNode->SetKeyProvider(std::unique_ptr<IKeyProvider>(new SimpleKeyProvider(userKey, contentKey)));
        return rootNode->DecryptNode(license, rootNode, m_cryptoProvider.get());
    }

    Status LcpService::DecryptLicenseByHexUserKey(ILicense * license, const std::string & hexUserKey)
    {
        KeyType userKey;
        Status res = m_cryptoProvider->ConvertHexToRaw(hexUserKey, userKey);
        if (!Status::IsSuccess(res))
            return res;
        return this->DecryptLicenseByUserKey(license, userKey);
    }

    Status LcpService::DecryptLicenseOnOpening(ILicense * license)
    {
        Status res = this->DecryptLicenseByStorage(license);
        if (Status::IsSuccess(res))
        {
            m_rightsService->SyncRightsFromStorage(license);
        }
        // if (Status::IsSuccess(res) || res.Code == StatusCode::ErrorDecryptionLicenseEncrypted)
        //     return Status(StatusCode::ErrorCommonSuccess);
        return res;
    }

    Status LcpService::AddDecryptedUserKey(ILicense * license, const KeyType & userKey)
    {
        std::string hexUserKey;
        Status res = m_cryptoProvider->ConvertRawToHex(userKey, hexUserKey);
        if (!Status::IsSuccess(res))
            return res;

        std::string userId = license->User()->Id();
        if (userId.empty())
        {
            userId = UnknownUserId;
        }
        return this->AddUserKey(hexUserKey, userId, license->Provider(), license->Id());
    }

    Status LcpService::DecryptLicenseByStorage(ILicense * license)
    {
        if (m_storageProvider == nullptr)
        {
            return Status(StatusCode::ErrorCommonNoStorageProvider);
        }

        std::string userKeyHex = m_storageProvider->GetValue(
            UserKeysVaultId,
            BuildStorageProviderKey(license)
            );
        if (!userKeyHex.empty())
        {
            Status res = this->DecryptLicenseByHexUserKey(license, userKeyHex);
            if (Status::IsSuccess(res)) {
                return res;
            }
        }

        std::unique_ptr<KvStringsIterator> it(m_storageProvider->EnumerateVault(UserKeysVaultId));
        for (it->First(); !it->IsDone(); it->Next())
        {
            Status res = this->DecryptLicenseByHexUserKey(license, it->Current());
            if (Status::IsSuccess(res))
                return res;
        }
        return Status(StatusCode::ErrorDecryptionLicenseEncrypted);
    }

    Status LcpService::DecryptData(
        ILicense * license,
        const unsigned char * data,
        const size_t dataLength,
        unsigned char * decryptedData,
        size_t * decryptedDataLength,
        const std::string & algorithm
        )
    {
        try
        {
            if (license == nullptr)
            {
                throw std::invalid_argument("license is nullptr");
            }

            if (!license->Decrypted())
            {
                return Status(StatusCode::ErrorDecryptionLicenseEncrypted);
            }
            
            IEncryptionProfile * profile = m_encryptionProfilesManager->GetProfile(license->Crypto()->EncryptionProfile());
            if (profile == nullptr)
            {
                return Status(StatusCode::ErrorCommonEncryptionProfileNotFound);
            }
            if (algorithm != profile->PublicationAlgorithm())
            {
                return Status(StatusCode::ErrorCommonAlgorithmMismatch);
            }

            IKeyProvider * keyProvider = dynamic_cast<IKeyProvider *>(license);
            if (keyProvider == nullptr)
            {
                throw std::logic_error("Can not cast ILicense to IKeyProvider");
            }

            return m_cryptoProvider->DecryptPublicationData(
                license,
                keyProvider,
                data,
                dataLength,
                decryptedData,
                decryptedDataLength
                );
        }
        catch (const StatusException & ex)
        {
            return ex.ResultStatus();
        }
    }

    Status LcpService::CreateEncryptedDataStream(
        ILicense * license,
        IReadableStream * stream,
        const std::string & algorithm,
        IEncryptedStream ** encStream
        )
    {
        try
        {
            if (license == nullptr || encStream == nullptr)
            {
                throw std::invalid_argument("wrong input params");
            }

            if (!license->Decrypted())
            {
                return Status(StatusCode::ErrorDecryptionLicenseEncrypted);
            }

            IEncryptionProfile * profile = m_encryptionProfilesManager->GetProfile(license->Crypto()->EncryptionProfile());
            if (profile == nullptr)
            {
                return Status(StatusCode::ErrorCommonEncryptionProfileNotFound);
            }
            if (algorithm != profile->PublicationAlgorithm())
            {
                return Status(StatusCode::ErrorCommonAlgorithmMismatch);
            }

            IKeyProvider * keyProvider = dynamic_cast<IKeyProvider *>(license);
            if (keyProvider == nullptr)
            {
                throw std::logic_error("Can not cast ILicense to IKeyProvider");
            }

            return m_cryptoProvider->CreateEncryptedPublicationStream(
                license,
                keyProvider,
                stream,
                encStream
                );
        }
        catch (const StatusException & ex)
        {
            return ex.ResultStatus();
        }
    }

    Status LcpService::AddUserKey(const std::string & userKey)
    {
        try
        {
            return this->AddUserKey(userKey, UnknownUserId, UnknownProvider);
        }
        catch (const StatusException & ex)
        {
            return ex.ResultStatus();
        }
    }

    Status LcpService::AddUserKey(
        const std::string & userKey,
        const std::string & userId,
        const std::string & providerId
        )
    {
        try
        {
            std::string randomLicenseId;
            Status res = m_cryptoProvider->GenerateUuid(randomLicenseId);
            if (!Status::IsSuccess(res))
            {
                return res;
            }
            return this->AddUserKey(userKey, userId, providerId, randomLicenseId);
        }
        catch (const StatusException & ex)
        {
            return ex.ResultStatus();
        }
    }

    Status LcpService::AddUserKey(
        const std::string & userKey,
        const std::string & userId,
        const std::string & providerId,
        const std::string & licenseId
        )
    {
        try
        {
            if (m_storageProvider == nullptr)
            {
                return Status(StatusCode::ErrorCommonNoStorageProvider);
            }

            m_storageProvider->SetValue(
                UserKeysVaultId,
                BuildStorageProviderKey(providerId, userId, licenseId),
                userKey
                );

            return Status(StatusCode::ErrorCommonSuccess);
        }
        catch (const StatusException & ex)
        {
            return ex.ResultStatus();
        }
    }

    Status LcpService::CreatePublicationAcquisition(
            const std::string & publicationPath,
            ILicense * license,
            IAcquisition ** acquisition
    )
    {
        try
        {
            if (acquisition == nullptr)
            {
                throw std::invalid_argument("acquisition is nullptr");
            }
            if (m_netProvider == nullptr)
            {
                return Status(StatusCode::ErrorCommonNoNetProvider);
            }
            *acquisition = new Acquisition(
                    license,
                    m_fileSystemProvider,
                    m_netProvider,
                    m_cryptoProvider.get(),
                    publicationPath
            );
            return Status(StatusCode::ErrorCommonSuccess);
        }
        catch (const StatusException & ex)
        {
            return ex.ResultStatus();
        }
    }

    Status LcpService::CreatePublicationStatusDocumentProcessing(
            const std::string & publicationPath,
            ILicense * license,
            IStatusDocumentProcessing ** statusDocumentProcessing
    )
    {
        try
        {
            if (statusDocumentProcessing == nullptr)
            {
                throw std::invalid_argument("statusDocumentProcessing is nullptr");
            }
            *statusDocumentProcessing = new StatusDocumentProcessing();
            return Status(StatusCode::ErrorCommonSuccess);
        }
        catch (const StatusException & ex)
        {
            return ex.ResultStatus();
        }
    }

    IRightsService * LcpService::GetRightsService() const
    {
        return m_rightsService.get();
    }

    std::string LcpService::RootCertificate() const
    {
        return m_rootCertificate;
    }

    INetProvider * LcpService::NetProvider() const
    {
        return m_netProvider;
    }

    IStorageProvider * LcpService::StorageProvider() const
    {
        return m_storageProvider;
    }

    IFileSystemProvider * LcpService::FileSystemProvider() const
    {
        return m_fileSystemProvider;
    }

    bool LcpService::FindLicense(const std::string & canonicalJson, ILicense ** license)
    {
        std::unique_lock<std::mutex> locker(m_licensesSync);

        auto licIt = m_licenses.find(canonicalJson);
        if (licIt != m_licenses.end())
        {
            *license = licIt->second.get();
            return true;
        }
        return false;
    }

    std::string LcpService::CalculateCanonicalForm(const std::string & licenseJson)
    {
        JsonCanonicalizer canonicalizer(licenseJson, m_jsonReader.get());
        return canonicalizer.CanonicalLicense();
    }

    std::string LcpService::BuildStorageProviderKey(
        const std::string & providerId,
        const std::string & userId,
        const std::string & licenseId
        )
    {
        std::stringstream keyStream;
        keyStream << providerId << "@" << userId << "@" << licenseId;
        return keyStream.str();
    }

    std::string LcpService::BuildStorageProviderKey(ILicense * license)
    {
        std::string userId = license->User()->Id();
        if (userId.empty())
        {
            userId = UnknownUserId;
        }
        std::stringstream keyStream;
        keyStream << license->Provider() << "@" << userId << "@" << license->Id();
        return keyStream.str();
    }
}
