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
#include "public/INetProvider.h"
#include "Acquisition.h"
#include "RightsService.h"
#include "SimpleMemoryWritableStream.h"
#include "DownloadInMemoryRequest.h"
#include "DateTime.h"
#include "ThreadTimer.h"

namespace lcp
{
    // << LSD
    /*static*/ std::string LcpService::StatusType = "application/vnd.readium.license.status.v1.0+json";
    // https://github.com/drminside/lsd_client/blob/master/json_schema_lsd.json
    // >> LSD

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
    {
    }

    Status LcpService::OpenLicense(const std::string & publicationPath, const std::string & licenseJson, std::promise<ILicense*> & licensePromise)
    {
        m_publicationPath = publicationPath;

        try
        {
            ILicense* license_;
            ILicense** license = &license_;
            
            if (license == nullptr)
            {
                throw std::invalid_argument("license is nullptr");
            }

            std::string canonicalJson = this->CalculateCanonicalForm(licenseJson);
            if (this->FindLicense(canonicalJson, license))
            {
                return Status(StatusCode::ErrorCommonSuccess);
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
            if (!Status::IsSuccess(res))
                return res;

            std::unique_lock<std::mutex> locker(m_licensesSync);
            auto insertRes = m_licenses.insert(std::make_pair(std::move(canonicalJson), std::move(rootNode)));
            if (!insertRes.second)
            {
                return Status(StatusCode::ErrorOpeningDuplicateLicenseInstance, "Two License instances with the same canonical form");
            }
            *license = insertRes.first->second.get();
            locker.unlock();

            Status result = Status(StatusCode::ErrorCommonSuccess);

            // << LSD
            result = this->ProcessLicenseStatusDocument(license, licensePromise);

            if (result.code == StatusCode::ErrorStatusDocumentNewLicense) {
                // result = Status(StatusCode::ErrorCommonSuccess);

                // Execution flow: on the client side of the LcpService::OpenLicense() API (i.e. LCP Content Module), the call to licensePromise.get() is blocking,
                // while the LSD downloader asynchronously acquires the updated license.
            } else {
            // >> LSD
                result = this->CheckDecrypted(*license);
                licensePromise.set_value(*license);
            }

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
    
    // << LSD

    // INetProviderCallback
    void LcpService::OnRequestStarted(INetRequest * request)
    {
        std::unique_lock<std::mutex> locker(m_lsdSync);
        m_lsdRequestRunning = true;
    }

    // INetProviderCallback
    void LcpService::OnRequestProgressed(INetRequest * request, float progress)
    {
        // noop
    }

    // INetProviderCallback
    void LcpService::OnRequestCanceled(INetRequest * request)
    {
        std::unique_lock<std::mutex> locker(m_lsdSync);
        m_lsdRequestRunning = false;
        m_lsdCondition.notify_one();
    }

    // INetProviderCallback
    void LcpService::OnRequestEnded(INetRequest * request, Status result)
    {
        std::unique_lock<std::mutex> locker(m_lsdSync);
        m_lsdRequestStatus = result;
        m_lsdRequestRunning = false;
        try
        {
            if (Status::IsSuccess(result))
            {
                Status hashCheckResult = this->CheckStatusDocumentHash(m_lsdStream); //m_lsdFile.get()
                if (Status::IsSuccess(hashCheckResult))
                {
                    //std::vector<unsigned char>
                    Buffer buffer = m_lsdStream->Buffer();
                    
                    if (!buffer.empty())
                    {
                        std::string bufferStr(buffer.begin(), buffer.end());

                        // C++ 11
                        //std::string bufferStr(buffer.data(), buffer.size());

                        // C++ 98
                        //std::string bufferStr(&buffer[0], buffer.size());

                        //TODO: parse JSON bufferStr

                        // TODO: download updated LCP licence
                        //m_lsdNewLcpLicenseString
                                
                        if (!m_lsdNewLcpLicenseString.empty())
                        {
                            std::stringstream licenseStream(m_lsdNewLcpLicenseString);
                            ZipFile::AddFile(m_publicationPath, licenseStream, LcpLicensePath);
                        }
                        
                    }
                }
                else 
                {
                    m_lsdRequestStatus = hashCheckResult;
                }
            }

            m_lsdCondition.notify_one();
        }
        catch (const std::exception & ex)
        {
            m_lsdRequestStatus = Status(StatusCode::ErrorNetworkingRequestFailed, ex.what());
            m_lsdCondition.notify_one();
        }
    }

    Status LcpService::CheckStatusDocumentHash(IReadableStream* stream)
    {
        if (!lsdLink.hash.empty())
        {
            std::vector<unsigned char> rawHash;
            Status res = m_cryptoProvider->CalculateFileHash(stream, rawHash);
            if (!Status::IsSuccess(res))
                return res;

            std::string hexHash;
            res = m_cryptoProvider->ConvertRawToHex(rawHash, hexHash);
            if (!Status::IsSuccess(res))
                return res;

            if (hexHash != lsdLink.hash)
            {
                return Status(StatusCode::ErrorStatusDocumentCorrupted);
            }
        }
        return Status(StatusCode::ErrorCommonSuccess);
    }

    // https://github.com/drminside/lsd_client/blob/master/lsd_client.py#L453
    std::string LcpService::ResolveTemplatedURL(const std::string & url)
    {
        // TODO
        return url;
    }

    // https://docs.google.com/document/d/1ErBf0Gl32jNH-QVKWpGPfZDMWMeQP7dH9YY5g7agguQ
    // https://docs.google.com/document/d/1VKkAG9aKbKLQYnSjSa5-_cOgnVFsgU9a7bHO0-IkRgk
    Status LcpService::ProcessLicenseStatusDocument(ILicense** license, std::promise<ILicense*> & licensePromise)
    {
        try
        {
            if (license == nullptr)
            {
                throw std::invalid_argument("license pointer is nullptr");
            }
            if (*license == nullptr)
            {
                throw std::invalid_argument("license is nullptr");
            }

            // if (!*license->Decrypted())
            // {
            //     return Status(StatusCode::ErrorDecryptionLicenseEncrypted);
            // }

            // RootLcpNode * rootNode = dynamic_cast<RootLcpNode *>(*license);
            // if (rootNode == nullptr)
            // {
            //     throw std::logic_error("Can not cast ILicense to ILcpNode / RootLcpNode");
            // }
            
            ILinks* links = *license->Links();

            //bool foundLink = links->GetLink(Status, m_lsdLink);
            //if (!foundLink) {
            if (!links->Has(StatusDocument))
            {
                //return Status(StatusCode::ErrorStatusDocumentNoStatusLink);
                return Status(StatusCode::ErrorCommonSuccess); // no LSD, noop
            }

            links->GetLink(StatusDocument, m_lsdLink);
            if (m_lsdLink.type != StatusType)
            {
                return Status(StatusCode::ErrorStatusDocumentWrongType);
            }

            if (!m_lsdLink.href.length())
            {
                return Status(StatusCode::ErrorStatusDocumentInvalidUri);
            }

            std::string url = m_lsdLink.href;
            if (m_lsdLink.templated)
            {
                url = this->ResolveTemplatedURL(url);
            }

            if (m_netProvider == nullptr)
            {
                return Status(StatusCode::ErrorCommonNoNetProvider);
            }
        
            std::unique_lock<std::mutex> locker(m_lsdSync);

            // m_lsdPath = "/tmp/lsd.json";
            // m_lsdFile.reset(m_fileSystemProvider->GetFile(m_lsdPath));
            // if (m_lsdFile.get() == nullptr)
            // {
            //     return Status(StatusCode::ErrorStatusDocumentInvalidFilePath);
            // }
            // m_lsdRequest.reset(new DownloadInFileRequest(m_lsdLink.href, m_lsdFile.get()));

            m_lsdStream.reset(new SimpleMemoryWritableStream());
            m_lsdRequest.reset(new DownloadInMemoryRequest(m_lsdLink.href, m_lsdStream.get()));

            locker.unlock();

            m_lsdRequestStatus = Status(StatusCode::ErrorNetworkingRequestFailed);

            m_lsdRequestRunning = false;
            m_netProvider->StartDownloadRequest(m_lsdRequest.get(), this);
            
            m_conditionDownload.wait(locker, [&]() { return !m_lsdRequestRunning; });

            if (Status::IsSuccess(m_lsdRequestStatus) || m_lsdRequest->Canceled())
            {
                // TODO
            }

            return Status(StatusCode::ErrorStatusDocumentNewLicense);
        }
        catch (const StatusException & ex)
        {
            return ex.ResultStatus();
        }
    }

    // >> LSD

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
