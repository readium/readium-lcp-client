#ifndef __LCP_SERVICE_H__
#define __LCP_SERVICE_H__

#include <map>
#include <memory>
#include "Public/ILcpService.h"
#include "LcpUtils.h"

namespace lcp
{
    class JsonValueReader;
    class EncryptionProfilesManager;
    class ICryptoProvider;
    class UUIDGenerator;

    class LcpService : public ILcpService
    {
    public:
        LcpService(
            const std::string & rootCertificate,
            INetProvider * netProvider,
            IStorageProvider * storageProvider
            );

        // ILcpService
        virtual Status OpenLicense(const std::string & licenseJson, ILicense ** license);

        virtual Status CreateDecryptionContext(IDecryptionContext ** decryptionContext);
        virtual Status DecryptLicense(ILicense * license, const std::string & userPassphrase);
        virtual Status DecryptData(
            ILicense * license,
            IDecryptionContext * context,
            const unsigned char * data,
            const size_t dataLength,
            unsigned char * decryptedData,
            size_t inDecryptedDataLength,
            size_t * outDecryptedDataLength,
            const std::string & algorithm
            );

        Status AddUserKey(const std::string & userKey);
        Status AddUserKey(
            const std::string & userKey,
            const std::string & userId,
            const std::string & providerId
            );

        virtual std::string RootCertificate() const;
        virtual INetProvider * NetProvider() const;
        virtual IStorageProvider * StorageProvider() const;
        
    private:
        bool FindLicense(const std::string & canonicalJson, ILicense ** license);

        Status DecryptLicenseByUserKey(ILicense * license, const KeyType & userKey);
        Status DecryptLicenseByStorage(ILicense * license);
        Status AddDecryptedUserKey(ILicense * license, const KeyType & userKey);

        std::string CalculateCanonicalForm(const std::string & licenseJson);
        std::string BuildStorageProviderKey(const std::string & part1, const std::string & part2);

    private:
        std::string m_rootCertificate;
        INetProvider * m_netProvider;
        IStorageProvider * m_storageProvider;

        std::unique_ptr<JsonValueReader> m_jsonReader;
        std::unique_ptr<EncryptionProfilesManager> m_encryptionProfilesManager;
        std::unique_ptr<ICryptoProvider> m_cryptoProvider;
        std::unique_ptr<UUIDGenerator> m_uuidGenerator;
        std::map<std::string, std::unique_ptr<ILicense> > m_licenses;

    private:
        static std::string UnknownProvider;
    };
}
#endif //__LCP_SERVICE_H__
