#include "LcpService.h"
#include "CryptoLcpNode.h"
#include "LinksLcpNode.h"
#include "UserLcpNode.h"
#include "RightsLcpNode.h"
#include "RootLcpNode.h"
#include "JsonValueReader.h"
#include "JsonCanonicalizer.h"
#include "EncryptionProfilesManager.h"
#include "CryptoppCryptoProvider.h"
#include "SimpleKeyProvider.h"
#include "UUIDGenerator.h"
#include "Public/IStorageProvider.h"
#include "Public/INetProvider.h"
#include "Acquisition.h"
#include "RightsService.h"

namespace lcp
{
    /*static*/ std::string LcpService::UnknownProvider = "UnknownProvider";
    /*static*/ std::string LcpService::UnknownUserId = "UnknownUserId";

    LcpService::LcpService(
        const std::string & rootCertificate,
        INetProvider * netProvider,
        IStorageProvider * storageProvider,
        IFileSystemProvider * fileSystemProvider
        )
        : m_rootCertificate(rootCertificate)
        , m_netProvider(netProvider)
        , m_storageProvider(storageProvider)
        , m_fileSystemProvider(fileSystemProvider)
        , m_rightsService(new RightsService(m_storageProvider, UnknownUserId))
        , m_jsonReader(new JsonValueReader())
        , m_encryptionProfilesManager(new EncryptionProfilesManager())
        , m_cryptoProvider(new CryptoppCryptoProvider(m_encryptionProfilesManager.get()))
        , m_uuidGenerator(new UUIDGenerator())
    {
    }

    Status LcpService::OpenLicense(const std::string & licenseJson, ILicense ** license)
    {
        try
        {
            if (license == nullptr)
            {
                return Status(StCodeCover::ErrorCommonFail, "license is nullptr");
            }

            std::string canonicalJson = this->CalculateCanonicalForm(licenseJson);
            if (this->FindLicense(canonicalJson, license))
            {
                return Status(Status(StCodeCover::ErrorCommonSuccess));
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

            auto insertRes = m_licenses.insert(std::make_pair(std::move(canonicalJson), std::move(rootNode)));
            if (!insertRes.second)
            {
                return Status(StCodeCover::ErrorCommonFail, "Two License instances with the same canonical form");
            }
            *license = insertRes.first->second.get();

            if (!(*license)->Decrypted())
            {
                res = this->DecryptLicenseByStorage(*license);
                if (Status::IsSuccess(res))
                {
                    m_rightsService->SyncRightsFromStorage(*license);
                }
                if (Status::IsSuccess(res) || res.ResultCode == StCodeCover::ErrorDecryptionLicenseEncrypted)
                    return Status(StCodeCover::ErrorCommonSuccess);
                return res;
            }
            return Status(StCodeCover::ErrorCommonSuccess);
        }
        catch (const StatusException & ex)
        {
            return ex.ResultStatus();
        }
        catch (const std::exception & ex)
        {
            return Status(StCodeCover::ErrorCommonFail, ex.what());
        }
    }

    Status LcpService::DecryptLicense(ILicense * license, const std::string & userPassphrase)
    {
        try
        {
            if (license == nullptr)
            {
                return Status(StCodeCover::ErrorCommonFail, "license is nullptr");
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
        catch (const std::exception & ex)
        {
            return Status(StCodeCover::ErrorCommonFail, ex.what());
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
            return Status(StCodeCover::ErrorCommonFail, "Can not cast ILicense to ILcpNode");
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

    Status LcpService::AddDecryptedUserKey(ILicense * license, const KeyType & userKey)
    {
        std::string hexUserKey;
        Status res = m_cryptoProvider->ConvertRawToHex(userKey, hexUserKey);
        if (!Status::IsSuccess(res))
            return res;

        std::string userId = license->User()->Id();
        if (!userId.empty())
        {
            res = this->AddUserKey(hexUserKey, userId, license->Provider(), license->Id());
        }
        else
        {
            res = this->AddUserKey(hexUserKey, UnknownUserId, license->Provider(), license->Id());
        }
        return res;
    }

    Status LcpService::DecryptLicenseByStorage(ILicense * license)
    {
        if (m_storageProvider == nullptr)
        {
            return Status(StCodeCover::ErrorCommonNoStorageProvider);
        }

        std::string userKeyHex = m_storageProvider->GetValue(
            UserKeysVaultId,
            BuildStorageProviderKey(license)
            );
        if (!userKeyHex.empty())
        {
            return this->DecryptLicenseByHexUserKey(license, userKeyHex);
        }

        std::unique_ptr<KvStringsIterator> it(m_storageProvider->EnumerateVault(UserKeysVaultId));
        for (it->First(); !it->IsDone(); it->Next())
        {
            Status res = this->DecryptLicenseByHexUserKey(license, it->Current());
            if (Status::IsSuccess(res))
                return res;
        }
        return Status(StCodeCover::ErrorDecryptionLicenseEncrypted);
    }

    Status LcpService::DecryptData(
        ILicense * license,
        const unsigned char * data,
        const size_t dataLength,
        unsigned char * decryptedData,
        size_t inDecryptedDataLength,
        size_t * outDecryptedDataLength,
        const std::string & algorithm
        )
    {
        try
        {
            if (license == nullptr)
            {
                return Status(StCodeCover::ErrorCommonFail, "license is nullptr");
            }

            if (!license->Decrypted())
            {
                return Status(StCodeCover::ErrorDecryptionLicenseEncrypted);
            }
            
            IEncryptionProfile * profile = m_encryptionProfilesManager->GetProfile(license->Crypto()->EncryptionProfile());
            if (profile == nullptr)
            {
                return Status(StCodeCover::ErrorCommonEncryptionProfileNotFound);
            }
            if (algorithm != profile->PublicationAlgorithm())
            {
                return Status(StCodeCover::ErrorCommonAlgorithmMismatch);
            }

            IKeyProvider * keyProvider = dynamic_cast<IKeyProvider *>(license);
            if (keyProvider == nullptr)
            {
                return Status(StCodeCover::ErrorCommonFail, "Can not cast ILicense to IKeyProvider");
            }

            return m_cryptoProvider->DecryptPublicationData(
                license,
                keyProvider,
                data,
                dataLength,
                decryptedData,
                inDecryptedDataLength,
                outDecryptedDataLength
                );
        }
        catch (const StatusException & ex)
        {
            return ex.ResultStatus();
        }
        catch (const std::exception & ex)
        {
            return Status(StCodeCover::ErrorCommonFail, ex.what());
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
                return Status(StCodeCover::ErrorCommonFail, "wrong input params");
            }

            if (!license->Decrypted())
            {
                return Status(StCodeCover::ErrorDecryptionLicenseEncrypted);
            }

            IEncryptionProfile * profile = m_encryptionProfilesManager->GetProfile(license->Crypto()->EncryptionProfile());
            if (profile == nullptr)
            {
                return Status(StCodeCover::ErrorCommonEncryptionProfileNotFound);
            }
            if (algorithm != profile->PublicationAlgorithm())
            {
                return Status(StCodeCover::ErrorCommonAlgorithmMismatch);
            }

            IKeyProvider * keyProvider = dynamic_cast<IKeyProvider *>(license);
            if (keyProvider == nullptr)
            {
                return Status(StCodeCover::ErrorCommonFail, "Can not cast ILicense to IKeyProvider");
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
        catch (const std::exception & ex)
        {
            return Status(StCodeCover::ErrorCommonFail, ex.what());
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
        catch (const std::exception & ex)
        {
            return Status(StCodeCover::ErrorCommonFail, ex.what());
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
            std::string randomLicenseId = m_uuidGenerator->GenerateUUID();
            return this->AddUserKey(userKey, userId, providerId, randomLicenseId);
        }
        catch (const StatusException & ex)
        {
            return ex.ResultStatus();
        }
        catch (const std::exception & ex)
        {
            return Status(StCodeCover::ErrorCommonFail, ex.what());
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
                return Status(StCodeCover::ErrorCommonNoStorageProvider);
            }

            m_storageProvider->SetValue(
                UserKeysVaultId,
                BuildStorageProviderKey(providerId, userId, licenseId),
                userKey
                );

            return Status(StCodeCover::ErrorCommonSuccess);
        }
        catch (const StatusException & ex)
        {
            return ex.ResultStatus();
        }
        catch (const std::exception & ex)
        {
            return Status(StCodeCover::ErrorCommonFail, ex.what());
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
                return Status(StCodeCover::ErrorCommonFail, "acquisition is nullptr");
            }
            if (m_netProvider == nullptr)
            {
                return Status(StCodeCover::ErrorCommonNoNetProvider);
            }
            *acquisition = new Acquisition(
                license,
                m_fileSystemProvider,
                m_netProvider,
                m_cryptoProvider.get(),
                publicationPath
                );
            return Status(StCodeCover::ErrorCommonSuccess);
        }
        catch (const StatusException & ex)
        {
            return ex.ResultStatus();
        }
        catch (const std::exception & ex)
        {
            return Status(StCodeCover::ErrorCommonFail, ex.what());
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