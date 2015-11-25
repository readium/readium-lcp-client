#ifndef __CRYPTO_ALGORITHM_INTERFACES_H__
#define __CRYPTO_ALGORITHM_INTERFACES_H__

#include <string>
#include <vector>
#include "LcpUtils.h"

namespace lcp
{
    class IDecryptionContext;

    class ISymmetricAlgorithm
    {
    public:
        virtual std::string Name() const = 0;

        virtual size_t Decrypt(
            const unsigned char * data,
            size_t dataLength,
            unsigned char * decryptedData,
            size_t decryptedDataLength,
            IDecryptionContext * context = nullptr
            ) = 0;

        virtual std::string Decrypt(
            const std::string & encryptedDataBase64,
            IDecryptionContext * context = nullptr
            ) = 0;

        virtual ~ISymmetricAlgorithm() {}
    };

    class IHashAlgorithm
    {
    public:
        virtual std::string Name() const = 0;
        virtual size_t DigestSize() const = 0;

        virtual KeyType CalculateHash(const std::string & dataStr) = 0;
        virtual KeyType CalculateHash(
            const unsigned char * data,
            const size_t dataLength
            ) = 0;
        
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
