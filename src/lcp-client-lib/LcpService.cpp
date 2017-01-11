// Copyright (c) 2016 Mantano
// Licensed to the Readium Foundation under one or more contributor license agreements.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation and/or
//    other materials provided with the distribution.
// 3. Neither the name of the organization nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#include "IncludeMacros.h"

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
#include "RightsService.h"

#include "DateTime.h"

#if ENABLE_NET_PROVIDER_ACQUISITION
#include "Acquisition.h"
#endif //ENABLE_NET_PROVIDER_ACQUISITION

ZIPLIB_INCLUDE_START
#include "ziplib/Source/ZipLib/ZipFile.h"
ZIPLIB_INCLUDE_END

static std::string const LcpLicensePath = "META-INF/license.lcpl";

namespace lcp
{
    /*static*/ std::string LcpService::UnknownProvider = "UnknownProvider";
    /*static*/ std::string LcpService::UnknownUserId = "UnknownUserId";

    LcpService::LcpService(
        const std::string & rootCertificate,
#if ENABLE_NET_PROVIDER
        INetProvider * netProvider,
#endif //ENABLE_NET_PROVIDER
        IStorageProvider * storageProvider,
        IFileSystemProvider * fileSystemProvider
#if !DISABLE_CRL
        , const std::string & defaultCrlUrl
#endif //!DISABLE_CRL
        )
        : m_rootCertificate(rootCertificate)
#if ENABLE_NET_PROVIDER
        , m_netProvider(netProvider)
#endif //ENABLE_NET_PROVIDER
        , m_storageProvider(storageProvider)
        , m_fileSystemProvider(fileSystemProvider)
        , m_rightsService(new RightsService(m_storageProvider, UnknownUserId))
        , m_jsonReader(new JsonValueReader())
        , m_encryptionProfilesManager(new EncryptionProfilesManager())
        , m_cryptoProvider(new CryptoppCryptoProvider(m_encryptionProfilesManager.get()
#if ENABLE_NET_PROVIDER
                    , m_netProvider
#endif //ENABLE_NET_PROVIDER

#if !DISABLE_CRL
                    , defaultCrlUrl
#endif //!DISABLE_CRL
            ))
    {
    }

    Status LcpService::InjectLicense(
            const std::string & publicationPath,
            const std::string & licenseJson) {
        try
        {
            std::stringstream licenseStream(licenseJson);
            ZipFile::AddFile(publicationPath, licenseStream, LcpLicensePath);
            return Status(StatusCode::ErrorCommonSuccess);
        }
        catch (const StatusException & ex)
        {
            return ex.ResultStatus();
        }
        catch (const std::exception & ex) {
            return Status(StatusCode::ErrorLicenseFailToInject, "ErrorLicenseFailToInject");
        }
    }

    Status LcpService::InjectLicense(
            const std::string & publicationPath,
            ILicense* license) {
        return this->InjectLicense(publicationPath, license->OriginalContent());
    }

    int LcpService::TimeStampCompare(const std::string & t1, const std::string & t2) {

        //DateTime now = DateTime::Now();
        DateTime time1(t1);
        DateTime time2(t2);
        if (time1 < time2) {
            return -1;
        } else if (time1 == time2) {
            return 0;
        } else if (time1 > time2) {
            return 1;
        } else {
            // What?!
            return 0;
        }
    }


    Status LcpService::OpenLicense(
            const std::string & publicationPath,
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
                    res = this->CheckDecrypted((*licensePTR));
                }

                res = this->CheckLicenseStatusDocument((*licensePTR));

                return res;
            }

            CryptoLcpNode* cryptoNode = new CryptoLcpNode(m_encryptionProfilesManager.get());
            LinksLcpNode* linksNode = new LinksLcpNode();
            UserLcpNode* userNode = new UserLcpNode();
            RightsLcpNode* rightsNode = new RightsLcpNode();

            std::unique_ptr<RootLcpNode> rootNode(new RootLcpNode(
                licenseJson,
                canonicalJson,
                    cryptoNode,
                    linksNode,
                    userNode,
                    rightsNode)
                );
#if ENABLE_GENERIC_JSON_NODE
            rootNode->AddChildNode(static_cast<ILcpNode*>(cryptoNode));
            rootNode->AddChildNode(static_cast<ILcpNode*>(linksNode));
            rootNode->AddChildNode(static_cast<ILcpNode*>(userNode));
            rootNode->AddChildNode(static_cast<ILcpNode*>(rightsNode));
#endif //ENABLE_GENERIC_JSON_NODE

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
                              "ErrorOpeningDuplicateLicenseInstance: Two License instances with the same canonical form");
            }
            (*licensePTR) = insertRes.first->second.get();
            locker.unlock();

            Status result = Status(StatusCode::ErrorCommonSuccess);

            result = this->CheckDecrypted((*licensePTR));

            result = this->CheckLicenseStatusDocument((*licensePTR));

            return result;
        }
        catch (const StatusException & ex)
        {
            return ex.ResultStatus();
        }
    }

    Status LcpService::CheckDecrypted(ILicense* license) {

        Status result = Status(StatusCode::ErrorCommonSuccess);

        if (
                false && // TODO comment this! => skips the decryption attempt (from stored passphrase) at first-time load, to test the user prompt
                !license->Decrypted())
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

    Status LcpService::CheckLicenseStatusDocument(ILicense* license)
    {
        if (license == nullptr)
        {
            return Status(StatusCode::ErrorCommonSuccess);
            //throw std::invalid_argument("license pointer is nullptr");
        }

        if (m_publicationPath.empty()) { // if a standalone LCPL, we wait until the linked EPUB is downloaded, then status doc will be checked.
            return Status(StatusCode::ErrorCommonSuccess);
        }

        try
        {
            // if (!license->Decrypted())
            // {
            //     return Status(StatusCode::ErrorDecryptionLicenseEncrypted, "ErrorDecryptionLicenseEncrypted");
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
                //return Status(StatusCode::ErrorStatusDocumentNoStatusLink, "ErrorStatusDocumentNoStatusLink");
            }

            lcp::Link lsdLink;
            links->GetLink(StatusDocument, lsdLink);

            std::string StatusType = "application/vnd.readium.license.status.v1.0+json";
            if (lsdLink.type != StatusType)
            {
                return Status(StatusCode::ErrorCommonSuccess); // bogus LSD link, noop
                //return Status(StatusCode::ErrorStatusDocumentWrongType, "ErrorStatusDocumentWrongType");
            }

            if (!lsdLink.href.length())
            {
                return Status(StatusCode::ErrorCommonSuccess); // bogus LSD link, noop
                //return Status(StatusCode::ErrorStatusDocumentInvalidUri, "ErrorStatusDocumentInvalidUri");
            }

            // Note that if the LCP license was updated following an LSD check
            // (any change that results in different canonical JSON string),
            // this "license" instance will be another one, even though they are associated with the same EPUB
            // (remember: m_licenses map keys are canonical JSON strings),
            // in which case another LSD check will be performed.
            if (license->getStatusDocumentProcessingFlag()) {
                license->setStatusDocumentProcessingFlag(false);

                // The LSD was checked at the last round, so now the LCP-EPUB is opening without LSD check.
                return Status(StatusCode::ErrorCommonSuccess);
            }

            // The LCP-EPUB will be loaded once again later, after LSD checks,
            // at which time we will need to bypass yet another LSD check,
            // to avoid infinite looping (see above "break")
            license->setStatusDocumentProcessingFlag(true);

            return Status(StatusCode::LicenseStatusDocumentStartProcessing, "LicenseStatusDocumentStartProcessing");
        }
        catch (const StatusException & ex)
        {
            return Status(StatusCode::ErrorCommonSuccess); // any LSD problem, noop
            //return ex.ResultStatus();
        }
        catch (const std::exception & ex)
        {
            return Status(StatusCode::ErrorCommonSuccess); // any LSD problem, noop
            // ex.what();
        }
    }

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
            return Status(StatusCode::ErrorCommonNoStorageProvider, "ErrorCommonNoStorageProvider");
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
        return Status(StatusCode::ErrorDecryptionLicenseEncrypted, "ErrorDecryptionLicenseEncrypted");
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
                return Status(StatusCode::ErrorDecryptionLicenseEncrypted, "ErrorDecryptionLicenseEncrypted");
            }
            
            IEncryptionProfile * profile = m_encryptionProfilesManager->GetProfile(license->Crypto()->EncryptionProfile());
            if (profile == nullptr)
            {
                return Status(StatusCode::ErrorCommonEncryptionProfileNotFound, "ErrorCommonEncryptionProfileNotFound");
            }
            if (algorithm != profile->PublicationAlgorithmGCM() && algorithm != profile->PublicationAlgorithmCBC())
            {
                return Status(StatusCode::ErrorCommonAlgorithmMismatch, "ErrorCommonAlgorithmMismatch");
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
                decryptedDataLength,
                algorithm
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
                return Status(StatusCode::ErrorDecryptionLicenseEncrypted, "ErrorDecryptionLicenseEncrypted");
            }

            IEncryptionProfile * profile = m_encryptionProfilesManager->GetProfile(license->Crypto()->EncryptionProfile());
            if (profile == nullptr)
            {
                return Status(StatusCode::ErrorCommonEncryptionProfileNotFound, "ErrorCommonEncryptionProfileNotFound");
            }

            if (algorithm != profile->PublicationAlgorithmGCM() && algorithm != profile->PublicationAlgorithmCBC())
            {
                return Status(StatusCode::ErrorCommonAlgorithmMismatch, "ErrorCommonAlgorithmMismatch");
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
                encStream,
                algorithm
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
                return Status(StatusCode::ErrorCommonNoStorageProvider, "ErrorCommonNoStorageProvider");
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

#if ENABLE_NET_PROVIDER_ACQUISITION

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
                return Status(StatusCode::ErrorCommonNoNetProvider, "ErrorCommonNoNetProvider");
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
#endif //ENABLE_NET_PROVIDER_ACQUISITION

    IRightsService * LcpService::GetRightsService() const
    {
        return m_rightsService.get();
    }

    std::string LcpService::RootCertificate() const
    {
        return m_rootCertificate;
    }

#if ENABLE_NET_PROVIDER
    INetProvider * LcpService::NetProvider() const
    {
        return m_netProvider;
    }
#endif //ENABLE_NET_PROVIDER

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
