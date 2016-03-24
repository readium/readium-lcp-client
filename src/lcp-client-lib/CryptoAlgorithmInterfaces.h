//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#ifndef __CRYPTO_ALGORITHM_INTERFACES_H__
#define __CRYPTO_ALGORITHM_INTERFACES_H__

#include <string>
#include <vector>
#include "LcpTypedefs.h"

namespace lcp
{
    class IDecryptionContext;
    class IReadableStream;

    class ISymmetricAlgorithm
    {
    public:
        virtual std::string Name() const = 0;

        virtual size_t Decrypt(
            const unsigned char * data,
            size_t dataLength,
            unsigned char * decryptedData,
            size_t decryptedDataLength
            ) = 0;

        virtual std::string Decrypt(
            const std::string & encryptedDataBase64
            ) = 0;

        virtual void Decrypt(
            IDecryptionContext * context,
            IReadableStream * stream,
            unsigned char * decryptedData,
            size_t decryptedDataLength
            ) = 0;

        virtual size_t PlainTextSize(IReadableStream * stream) = 0;

        virtual ~ISymmetricAlgorithm() {}
    };

    class IHashAlgorithm
    {
    public:
        virtual std::string Name() const = 0;
        virtual size_t DigestSize() const = 0;
        virtual void UpdateHash(const std::string & dataStr) = 0;
        virtual void UpdateHash(const unsigned char * data, const size_t dataLength) = 0;
        virtual KeyType Hash() = 0;
        
        virtual ~IHashAlgorithm() {}
    };

    class ISignatureAlgorithm
    {
    public:
        virtual std::string Name() const = 0;

        virtual bool VerifySignature(
            const std::string & message,
            const std::string & signatureBase64
            ) = 0;

        virtual bool VerifySignature(
            const unsigned char * message,
            size_t messageLength,
            const unsigned char * signature,
            size_t signatureLength
            ) = 0;

        virtual ~ISignatureAlgorithm() {}
    };
}

#endif //__CRYPTO_ALGORITHM_INTERFACES_H__
