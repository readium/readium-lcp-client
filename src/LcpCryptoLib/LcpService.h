#ifndef __LCP_SERVICE_H__
#define __LCP_SERVICE_H__

#include <map>
#include <memory>
#include "Public/ILcpService.h"

namespace lcp
{
    class JsonValueReader;
    class EncryptionProfilesManager;
    class ICryptoProvider;

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

        virtual Status DecryptLicense(ILicense * license, const std::string & userPassphrase);
        virtual Status DecryptData(
            ILicense * license,
            const unsigned char * data,
            const size_t dataLength,
            unsigned char * decryptedData,
            size_t inDecryptedDataLength,
            size_t * outDecryptedDataLength,
            const std::string & algorithm,
            bool firstDataBlock = true
            );

        virtual std::string RootCertificate() const;
        virtual INetProvider * NetProvider() const;
        virtual IStorageProvider * StorageProvider() const;
        
    private:
        bool FindLicense(const std::string & canonicalJson, ILicense ** license);
        std::string CalculateCanonicalForm(const std::string & licenseJson);

    private:
        std::string m_rootCertificate;
        INetProvider * m_netProvider;
        IStorageProvider * m_storageProvider;

        std::unique_ptr<JsonValueReader> m_jsonReader;
        std::unique_ptr<EncryptionProfilesManager> m_encryptionProfilesManager;
        std::unique_ptr<ICryptoProvider> m_cryptoProvider;

        std::map<std::string, std::unique_ptr<ILicense> > m_licenses;
    };
}
#endif //__LCP_SERVICE_H__
