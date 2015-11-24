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

namespace lcp
{
    /*static*/ std::string LcpService::UnknownProvider = "UnknownProvider";

    LcpService::LcpService(
        const std::string & rootCertificate,
        INetProvider * netProvider,
        IStorageProvider * storageProvider
        )
        : m_rootCertificate(rootCertificate)
        , m_netProvider(netProvider)
        , m_storageProvider(storageProvider)
        , m_jsonReader(new JsonValueReader())
        , m_encryptionProfilesManager(new EncryptionProfilesManager())
        , m_cryptoProvider(new CryptoppCryptoProvider(m_encryptionProfilesManager.get()))
        , m_uuidGenerator(new UUIDGenerator())
    {
    }

    Status LcpService::OpenLicense(const std::string & licenseJson, ILicense ** license)
    {
        if (license == nullptr)
        {
            throw std::invalid_argument("license is nullptr");
        }

        try
        {
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
                return Status(StCodeCover::ErrorCommonError, "Two License instances with the same canonical form");
            }
            *license = insertRes.first->second.get();
            
            if (!(*license)->Decrypted())
            {
                return this->DecryptLicenseByStorage(*license);
            }
            return Status(StCodeCover::ErrorCommonSuccess);
        }
        catch (const StatusException & ex)
        {
            return ex.ResultStatus();
        }
        catch (const std::exception & ex)
        {
            return Status(StCodeCover::ErrorCommonError, ex.what());
        }
    }

    Status LcpService::DecryptLicense(ILicense * license, const std::string & userPassphrase)
    {
        try
        {
            KeyType userKey;
            Status res = m_cryptoProvider->DecryptUserKey(userPassphrase, license, userKey);
            if (!Status::IsSuccess(res))
                return res;

            return this->DecryptLicenseByUserKey(license, userKey);
        }
        catch (const StatusException & ex)
        {
            return ex.ResultStatus();
        }
        catch (const std::exception & ex)
        {
            return Status(StCodeCover::ErrorCommonError, ex.what());
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
            return Status(StCodeCover::ErrorCommonError, "Can not cast ILicense to ILcpNode");
        }

        rootNode->SetKeyProvider(std::unique_ptr<IKeyProvider>(new SimpleKeyProvider(userKey, contentKey)));
        res = rootNode->DecryptNode(license, rootNode, m_cryptoProvider.get());
        if (!Status::IsSuccess(res))
            return res;

        /*std::string hexUserKey;
        res = m_cryptoProvider->ConvertKeyToHex(userKey, hexUserKey);
        if (!Status::IsSuccess(res))
        return res;*/

        std::string userId = license->User()->Id();
        if (!userId.empty())
        {
            res = this->AddUserKey(std::string(userKey.begin(), userKey.end()), userId, license->Provider());
        }
        else
        {
            res = this->AddUserKey(std::string(userKey.begin(), userKey.end()));
        }
        return res;
    }

    Status LcpService::DecryptData(
        ILicense * license,
        const unsigned char * data,
        const size_t dataLength,
        unsigned char * decryptedData,
        size_t inDecryptedDataLength,
        size_t * outDecryptedDataLength,
        const std::string & algorithm,
        bool firstDataBlock
        )
    {
        try
        {
            IEncryptionProfile * profile = m_encryptionProfilesManager->GetProfile(license->Crypto()->EncryptionProfile());
            if (algorithm != profile->PublicationAlgorithm())
            {
                return Status(StCodeCover::ErrorCommonAlgorithmMismatch);
            }

            IKeyProvider * keyProvider = dynamic_cast<IKeyProvider *>(license);
            if (keyProvider == nullptr)
            {
                return Status(StCodeCover::ErrorCommonError, "Can not cast ILicense to IKeyProvider");
            }

            return m_cryptoProvider->DecryptPublicationData(
                license,
                keyProvider,
                data,
                dataLength,
                decryptedData,
                inDecryptedDataLength,
                outDecryptedDataLength,
                firstDataBlock
                );
        }
        catch (const StatusException & ex)
        {
            return ex.ResultStatus();
        }
        catch (const std::exception & ex)
        {
            return Status(StCodeCover::ErrorCommonError, ex.what());
        }
    }

    Status LcpService::AddUserKey(const std::string & userKey)
    {
        try
        {
            if (m_storageProvider == nullptr)
            {
                return Status(StCodeCover::ErrorCommonNoStorageProvider);
            }

            std::string guid = m_uuidGenerator->GenerateUUID();
            m_storageProvider->SetValue(
                UserKeysVaultId,
                BuildStorageProviderKey(UnknownProvider, guid),
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
            return Status(StCodeCover::ErrorCommonError, ex.what());
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
            if (m_storageProvider == nullptr)
            {
                return Status(StCodeCover::ErrorCommonNoStorageProvider);
            }

            m_storageProvider->SetValue(
                UserKeysVaultId,
                BuildStorageProviderKey(providerId, userId),
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
            return Status(StCodeCover::ErrorCommonError, ex.what());
        }
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

    Status LcpService::DecryptLicenseByStorage(ILicense * license)
    {
        if (m_storageProvider == nullptr)
        {
            return Status(StCodeCover::ErrorCommonNoStorageProvider);
        }

        std::string userId = license->User()->Id();
        if (!userId.empty())
        {
            std::string userKeyStr = m_storageProvider->GetValue(
                UserKeysVaultId,
                BuildStorageProviderKey(license->Provider(), userId)
                );
            if (!userKeyStr.empty())
            {
                KeyType userKey;
                userKey.assign(userKeyStr.begin(), userKeyStr.end());
                return this->DecryptLicenseByUserKey(license, userKey);
            }
        }

        std::unique_ptr<MapIterator<std::string> > it(m_storageProvider->EnumerateVault(UserKeysVaultId));
        for (it->First(); !it->IsDone(); it->Next())
        {
            KeyType userKey;
            userKey.assign(it->Current()->second.begin(), it->Current()->second.end());
            Status res = this->DecryptLicenseByUserKey(license, userKey);
            if (Status::IsSuccess(res))
            {
                return res;
            }
        }
        return Status(StCodeCover::ErrorOpeningLicenseStillEncrypted);
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

    std::string LcpService::BuildStorageProviderKey(const std::string & part1, const std::string & part2)
    {
        return part1 + "@" + part2;
    }
}