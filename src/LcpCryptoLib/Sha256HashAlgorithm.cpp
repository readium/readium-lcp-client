#include <cryptopp/sha.h>
#include <cryptopp/filters.h>
#include "AlgorithmNames.h"
#include "Sha256HashAlgorithm.h"
#include "CryptoppUtils.h"

namespace lcp
{
    std::string Sha256HashAlgorithm::Name() const
    {
        return AlgorithmNames::Sha256Id;
    }

    size_t Sha256HashAlgorithm::DigestSize() const
    {
        return CryptoPP::SHA256::DIGESTSIZE;
    }

    KeyType Sha256HashAlgorithm::CalculateHash(const std::string & base64)
    {
        CryptoPP::SecByteBlock rawData;
        CryptoppUtils::Base64ToSecBlock(base64, rawData);
        return this->CalculateHash(rawData.data(), rawData.size());
    }

    KeyType Sha256HashAlgorithm::CalculateHash(
        const unsigned char * data,
        const size_t dataLength
        )
    {
        KeyType hash(CryptoPP::SHA256::DIGESTSIZE);

        CryptoPP::SHA256 sha256;
        CryptoPP::ArraySource dataSource(
            data, dataLength, true,
            new CryptoPP::HashFilter(
                sha256,
                new CryptoPP::ArraySink(
                    hash.data(), hash.size()
                    )
                )
            );

        return hash;
    }
}