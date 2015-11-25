#include <cryptopp/filters.h>
#include "AesCbcSymmetricAlgorithm.h"
#include "AlgorithmNames.h"
#include "CryptoppUtils.h"
#include "DecryptionContextImpl.h"

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
        IDecryptionContext * context
        )
    {
        CryptoPP::SecByteBlock rawData;
        CryptoppUtils::Base64ToSecBlock(encryptedDataBase64, rawData);

        const unsigned char * cipherData = rawData.data();
        size_t cipherSize = rawData.size();

        this->ProcessDecryptionContext(
            context,
            rawData.data(),
            rawData.size(),
            &cipherData,
            &cipherSize
            );
            
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
        IDecryptionContext * context
        )
    {
        const unsigned char * cipherData = data;
        size_t cipherSize = dataLength;

        this->ProcessDecryptionContext(
            context,
            data,
            dataLength,
            &cipherData,
            &cipherSize
            );

        CryptoPP::StreamTransformationFilter filter(m_decryptor);
        filter.Put(cipherData, cipherSize);

        filter.MessageEnd();
        size_t resultSize = static_cast<size_t>(filter.MaxRetrievable());

        if (decryptedDataLength < resultSize)
        {
            throw std::invalid_argument("decrypted data buffer is too small");
        }

        filter.Get(decryptedData, resultSize);
        return resultSize;
    }

    void AesCbcSymmetricAlgorithm::ProcessDecryptionContext(
        IDecryptionContext * context,
        const unsigned char * data,
        size_t dataLength,
        const unsigned char ** cipherData,
        size_t * cipherSize
        )
    {
        if (context != nullptr)
        {
            DecryptionContextImpl * contextImpl = dynamic_cast<DecryptionContextImpl *>(context);
            if (contextImpl == nullptr)
            {
                throw std::runtime_error("Cannot cast IDecryptionContext to DecryptionContextImpl");
            }

            if (contextImpl->IsFirstRange())
            {
                KeyType iv = this->BuildIV(data, dataLength, cipherData, cipherSize);
                m_decryptor.SetKeyWithIV(&m_key.at(0), m_key.size(), &iv.at(0));
                contextImpl->SetIV(iv);
            }
            else if (contextImpl->HasIV())
            {
                m_decryptor.SetKeyWithIV(&m_key.at(0), m_key.size(), &contextImpl->IV().at(0));
            }
        }
        else
        {
            KeyType iv = this->BuildIV(data, dataLength, cipherData, cipherSize);
            m_decryptor.SetKeyWithIV(&m_key.at(0), m_key.size(), &iv.at(0));
        }
    }

    KeyType AesCbcSymmetricAlgorithm::BuildIV(
        const unsigned char * data,
        size_t dataLength,
        const unsigned char ** cipherData,
        size_t * cipherSize
        )
    {
        // Length of block equals to length of IV
        size_t blockAndIvSize = CryptoPP::AES::BLOCKSIZE;
        KeyType iv;
        iv.resize(blockAndIvSize);

        if (dataLength < blockAndIvSize + blockAndIvSize)
        {
            throw std::invalid_argument("input data to decrypt is too small");
        }

        iv.assign(data, data + blockAndIvSize);
        *cipherData = data + blockAndIvSize;
        *cipherSize = dataLength - blockAndIvSize;

        return iv;
    }
}
