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

        virtual Status CalculateFileHash(
            IReadableStream * readableStream,
            std::vector<unsigned char> & rawHash
            );

        virtual Status ConvertRawToHex(
            const std::vector<unsigned char> & data,
            std::string & hex
            );

        virtual Status ConvertHexToRaw(
            const std::string & hex,
            std::vector<unsigned char> & data
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
            size_t * outDecryptedDataLength
            );

        virtual Status CreateEncryptedPublicationStream(
            ILicense * license,
            IKeyProvider * keyProvider,
            IReadableStream * stream,
            IEncryptedStream ** encStream
            );

    private:
        EncryptionProfilesManager * m_encryptionProfilesManager;
    };
}

#endif //__CRYPTOPP_CRYPTO_PROVIDER_H__
