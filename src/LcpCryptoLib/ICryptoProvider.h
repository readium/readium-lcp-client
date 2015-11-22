#ifndef __I_CRYPTO_PROVIDER_H__
#define __I_CRYPTO_PROVIDER_H__

#include "Public/LcpStatus.h"
#include "LcpUtils.h"

namespace lcp
{
    class ILicense;
    class IKeyProvider;

    class ICryptoProvider
    {
    public:
        virtual Status VerifyLicense(
            const std::string & rootCertificateBase64,
            ILicense * license
            ) = 0;

        virtual Status DecryptUserKey(
            const std::string & userPassphrase,
            ILicense * license,
            KeyType & userKey
            ) = 0;

        virtual Status DecryptContentKey(
            const KeyType & userKey,
            ILicense * license,
            KeyType & contentKey
            ) = 0;

        virtual Status DecryptLicenseData(
            const std::string & dataBase64,
            ILicense * license,
            IKeyProvider * keyProvider,
            std::string & decrypted
            ) = 0;

        virtual Status DecryptPublicationData(
            ILicense * license,
            IKeyProvider * keyProvider,
            const unsigned char * data,
            const size_t dataLength,
            unsigned char * decryptedData,
            size_t inDecryptedDataLength,
            size_t * outDecryptedDataLength,
            bool containsIv
            ) = 0;

        virtual ~ICryptoProvider() {}
    };
}

#endif //__I_CRYPTO_PROVIDER_H__