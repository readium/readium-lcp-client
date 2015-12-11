//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright � 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#ifndef __CRYPTOPP_CRYPTO_PROVIDER_H__
#define __CRYPTOPP_CRYPTO_PROVIDER_H__

#include <memory>
#include <mutex>
#include "ICryptoProvider.h"
#include "NonCopyable.h"

namespace lcp
{
    class EncryptionProfilesManager;
    class INetProvider;
    class ICertificate;
    class ICertificateRevocationList;
    class CrlUpdater;
    class ThreadTimer;

    class CryptoppCryptoProvider : public ICryptoProvider, public NonCopyable
    {
    public:
        CryptoppCryptoProvider(
            EncryptionProfilesManager * encryptionProfilesManager,
            INetProvider * netProvider,
            const std::string & defaultCrlUrl
            );
        ~CryptoppCryptoProvider();

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

        virtual Status GenerateUuid(std::string & uuid);

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
            size_t * decryptedDataLength
            );

        virtual Status CreateEncryptedPublicationStream(
            ILicense * license,
            IKeyProvider * keyProvider,
            IReadableStream * stream,
            IEncryptedStream ** encStream
            );

    private:
        Status ProcessRevokation(ICertificate * rootCertificate, ICertificate * providerCertificate);

    private:
        std::unique_ptr<ICertificateRevocationList> m_revocationList;
        std::unique_ptr<ThreadTimer> m_threadTimer;
        std::unique_ptr<CrlUpdater> m_crlUpdater;
        std::mutex m_processRevocationSync;
        EncryptionProfilesManager * m_encryptionProfilesManager;
    };
}

#endif //__CRYPTOPP_CRYPTO_PROVIDER_H__
