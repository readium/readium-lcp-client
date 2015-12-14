//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#ifndef __I_CRYPTO_PROVIDER_H__
#define __I_CRYPTO_PROVIDER_H__

#include "Public/LcpStatus.h"
#include "LcpTypedefs.h"

namespace lcp
{
    class ILicense;
    class IKeyProvider;
    class IReadableStream;
    class IEncryptedStream;

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

        virtual Status CalculateFileHash(
            IReadableStream * readableStream,
            std::vector<unsigned char> & rawHash
            ) = 0;

        virtual Status ConvertRawToHex(
            const std::vector<unsigned char> & data,
            std::string & hex
            ) = 0;

        virtual Status ConvertHexToRaw(
            const std::string & hex,
            std::vector<unsigned char> & key
            ) = 0;

        virtual Status GenerateUuid(std::string & uuid) = 0;

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
            size_t * decryptedDataLength
            ) = 0;

        virtual Status CreateEncryptedPublicationStream(
            ILicense * license,
            IKeyProvider * keyProvider,
            IReadableStream * stream,
            IEncryptedStream ** encStream
            ) = 0;

        virtual ~ICryptoProvider() {}
    };
}

#endif //__I_CRYPTO_PROVIDER_H__