#include <cryptopp/filters.h>
#include "AesCbcSymmetricAlgorithm.h"
#include "AlgorithmNames.h"
#include "CryptoppUtils.h"

namespace lcp
{
    AesCbcSymmetricAlgorithm::AesCbcSymmetricAlgorithm(
        const KeyType & key,
        KeySize keySize
        )
        : m_key(key)
        , m_keySize(keySize)
    {
        KeyType emptyIv(CryptoPP::AES::BLOCKSIZE);
        m_decryptor.SetKeyWithIV(&key.at(0), key.size(), &emptyIv.at(0));
    }

    std::string AesCbcSymmetricAlgorithm::Name() const
    {
        switch (m_keySize)
        {
        case lcp::AesCbcSymmetricAlgorithm::Key128:
            throw std::logic_error("not implemented");
        case lcp::AesCbcSymmetricAlgorithm::Key192:
            throw std::logic_error("not implemented");
        case lcp::AesCbcSymmetricAlgorithm::Key256:
            return AlgorithmNames::AesCbc256Id;
        default:
            throw std::logic_error("not implemented");
        }
    }

    std::string AesCbcSymmetricAlgorithm::Decrypt(
        const std::string & encryptedDataBase64,
        bool containsIv
        )
    {
        CryptoPP::SecByteBlock rawData;
        CryptoppUtils::Base64ToSecBlock(encryptedDataBase64, rawData);

        const unsigned char * cipherData = rawData.data();
        size_t cipherSize = rawData.size();
        if (containsIv)
        {
            this->BuildIV(rawData.data(), rawData.size(), &cipherData, &cipherSize);
        }

        std::string decryptedDataStr;
        
        CryptoPP::ArraySource source(
            cipherData, cipherSize, true,
            new CryptoPP::StreamTransformationFilter(m_decryptor,
                new CryptoPP::StringSink(decryptedDataStr)
                )
            );

        return decryptedDataStr;
    }

    size_t AesCbcSymmetricAlgorithm::Decrypt(
        const unsigned char * data,
        size_t dataLength,
        unsigned char * decryptedData,
        size_t decryptedDataLength,
        bool containsIv
        )
    {
        const unsigned char * cipherData = data;
        size_t cipherSize = dataLength;
        if (containsIv)
        {
            this->BuildIV(data, dataLength, &cipherData, &cipherSize);
        }

        CryptoPP::StreamTransformationFilter filter(m_decryptor);
        size_t put = filter.Put(cipherData, cipherSize);
        assert(put == cipherSize);

        filter.MessageEnd();
        size_t resultSize = static_cast<size_t>(filter.MaxRetrievable());

        if (resultSize < decryptedDataLength)
        {
            throw std::invalid_argument("decrypted data buffer is too small");
        }

        filter.Get(decryptedData, resultSize);
        return resultSize;
    }

    void AesCbcSymmetricAlgorithm::BuildIV(
        const unsigned char * data,
        size_t dataLength,
        const unsigned char ** cipherData,
        size_t * cipherSize
        )
    {
        // Length of block equals to length of IV
        size_t blockAndIvSize = CryptoPP::AES::BLOCKSIZE;
        CryptoPP::SecByteBlock iv(blockAndIvSize);

        if (dataLength < blockAndIvSize + blockAndIvSize)
        {
            throw std::invalid_argument("input data to decrypt is too small");
        }

        iv.Assign(data, blockAndIvSize);
        *cipherData = data + blockAndIvSize;
        *cipherSize = dataLength - blockAndIvSize;

        m_decryptor.SetKeyWithIV(&m_key.at(0), m_key.size(), iv.data());
    }
}
