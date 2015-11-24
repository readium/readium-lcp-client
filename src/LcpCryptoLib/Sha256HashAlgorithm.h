#ifndef __SHA256_HASH_ALGORITHM_H__
#define __SHA256_HASH_ALGORITHM_H__

#include "CryptoAlgorithmInterfaces.h"

namespace lcp
{
    class Sha256HashAlgorithm : public IHashAlgorithm
    {
    public:
        virtual std::string Name() const;
        virtual size_t DigestSize() const;
        virtual KeyType CalculateHash(const std::string & dataStr);
        virtual KeyType CalculateHash(
            const unsigned char * data,
            const size_t dataLength
            );
    };
}

#endif //__SHA256_HASH_ALGORITHM_H__
