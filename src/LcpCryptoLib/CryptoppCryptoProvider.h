#ifndef __CRYPTOPP_CRYPTO_PROVIDER_H__
#define __CRYPTOPP_CRYPTO_PROVIDER_H__

#include "ICryptoProvider.h"

namespace lcp
{
    class EncryptionProfilesManager;

    class CryptoppCryptoProvider : public ICryptoProvider
    {
    public:
        explicit CryptoppCryptoProvider(EncryptionProfilesManager * encryptionProfilesManager);

        virtual Status VerifyLicense(
            const std::string & rootCertificateBase64,
            ILicense * license
            );

        virtual Status DecryptUserKey(
            const std::string & userPassphrase,
            ILicense * license,
            KeyType & userKey
            );

        virtual Status DecryptContentKey(
            const KeyType & userKey,
            ILicense * license,
            KeyType & contentKey
            );

        virtual Status ConvertKeyToHex(
            const KeyType & key,
            std::string & hex
            );

        virtual Status ConvertHexToKey(
            const std::string & hex,
            KeyType & key
            );

        virtual Status DecryptLicenseData(
            const std::string & dataBase64,
            ILicense * license,
            IKeyProvider * keyProvider,
            std::string & decrypted
            );

        virtual Status DecryptPublicationData(
            ILicense * license,
            IKeyProvider * keyProvider,
            const unsigned char * data,
            const size_t dataLength,
            unsigned char * decryptedData,
            size_t inDecryptedDataLength,
            size_t * outDecryptedDataLength,
            bool containsIv
            );

    private:
        EncryptionProfilesManager * m_encryptionProfilesManager;
    };
}

#endif //__CRYPTOPP_CRYPTO_PROVIDER_H__
