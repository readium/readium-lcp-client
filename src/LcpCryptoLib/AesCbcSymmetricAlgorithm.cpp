#include "AesCbcSymmetricAlgorithm.h"
#include "AlgorithmNames.h"
#include "CryptoppUtils.h"
#include "IDecryptionContext.h"
#include "Public/StreamInterfaces.h"

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
        const std::string & encryptedDataBase64
        )
    {
        CryptoPP::SecByteBlock rawData;
        CryptoppUtils::Base64ToSecBlock(encryptedDataBase64, rawData);

        const unsigned char * cipherData = rawData.data();
        size_t cipherSize = rawData.size();

        KeyType iv = this->BuildIV(rawData.data(), rawData.size(), &cipherData, &cipherSize);
        m_decryptor.SetKeyWithIV(&m_key.at(0), m_key.size(), &iv.at(0));


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
        size_t decryptedDataLength
        )
    {
        return this->InnerDecrypt(
            data,
            dataLength,
            decryptedData,
            decryptedDataLength,
            BlockPaddingSchemeDef::DEFAULT_PADDING
            );
    }

    size_t AesCbcSymmetricAlgorithm::PlainTextSize(IReadableStream * stream)
    {
        if (stream->Size() < CryptoPP::AES::BLOCKSIZE + CryptoPP::AES::BLOCKSIZE)
        {
            throw std::out_of_range("Invalid encrypted file, size is out of range");
        }

        size_t readPosition = stream->Size() - (CryptoPP::AES::BLOCKSIZE + CryptoPP::AES::BLOCKSIZE);
        stream->SetReadPosition(readPosition);
        std::vector<unsigned char> inBuffer(CryptoPP::AES::BLOCKSIZE + CryptoPP::AES::BLOCKSIZE);
        std::vector<unsigned char> outBuffer(inBuffer.size());
        stream->Read(&inBuffer.at(0), inBuffer.size());

        size_t outSize = this->InnerDecrypt(
            &inBuffer.at(0),
            inBuffer.size(),
            &outBuffer.at(0),
            outBuffer.size(),
            BlockPaddingSchemeDef::DEFAULT_PADDING
            );

        return stream->Size()
            - CryptoPP::AES::BLOCKSIZE // minus IV or previous block
            - (CryptoPP::AES::BLOCKSIZE - outSize) % CryptoPP::AES::BLOCKSIZE; // minus padding part
    }

    void AesCbcSymmetricAlgorithm::Decrypt(
        IDecryptionContext * context,
        IReadableStream * stream,
        unsigned char * decryptedData,
        size_t decryptedDataLength
        )
    {
        DecRangeInfo rangeInfo = context->GetDecryptionRange();
        size_t plainTextSize = this->PlainTextSize(stream);
        if (rangeInfo.position + rangeInfo.length > plainTextSize)
        {
            throw std::out_of_range("params to decrypt out of range");
        }

        // Get offset result offset in the block
        size_t blockOffset = rangeInfo.position % CryptoPP::AES::BLOCKSIZE;
        // For beginning of the cipher text, IV used for XOR
        // For cipher text in the middle, previous block used for XOR
        size_t readPosition = rangeInfo.position - blockOffset;

        // Count blocks to read
        // First block for IV or previous block to perform XOR
        size_t blocksCount = 1;
        size_t bytesInFirstBlock = (CryptoPP::AES::BLOCKSIZE - blockOffset) % CryptoPP::AES::BLOCKSIZE;
        if (rangeInfo.length < bytesInFirstBlock)
        {
            bytesInFirstBlock = 0;
        }
        if (bytesInFirstBlock > 0)
        {
            blocksCount++;
        }

        blocksCount += (rangeInfo.length - bytesInFirstBlock) / CryptoPP::AES::BLOCKSIZE;
        if ((rangeInfo.length - bytesInFirstBlock) % CryptoPP::AES::BLOCKSIZE != 0)
        {
            blocksCount++;
        }

        // Figure out what padding scheme to use 
        BlockPaddingSchemeDef::BlockPaddingScheme padding = BlockPaddingSchemeDef::NO_PADDING;
        size_t sizeWithoutPaddedBlock = plainTextSize - (plainTextSize % CryptoPP::AES::BLOCKSIZE);
        if (rangeInfo.position + rangeInfo.length > sizeWithoutPaddedBlock)
        {
            padding = BlockPaddingSchemeDef::DEFAULT_PADDING;
        }

        // Read data from the stream
        stream->SetReadPosition(readPosition);
        std::vector<unsigned char> inBuffer(blocksCount * CryptoPP::AES::BLOCKSIZE);
        std::vector<unsigned char> outBuffer(inBuffer.size());
        if (readPosition + inBuffer.size() > stream->Size())
        {
            throw std::out_of_range("encrypted stream is out of range");
        }
        stream->Read(&inBuffer.at(0), inBuffer.size());

        // Decrypt and copy necessary data
        size_t outSize = this->InnerDecrypt(
            &inBuffer.at(0),
            inBuffer.size(),
            &outBuffer.at(0),
            outBuffer.size(),
            padding
            );

        if (outSize < rangeInfo.length)
        {
            throw std::out_of_range("range length is out of range");
        }

        outBuffer.resize(outSize);
        memcpy_s(decryptedData, decryptedDataLength, &outBuffer.at(blockOffset), rangeInfo.length);
    }

    size_t AesCbcSymmetricAlgorithm::InnerDecrypt(
        const unsigned char * data,
        size_t dataLength,
        unsigned char * decryptedData,
        size_t decryptedDataLength,
        CryptoPP::BlockPaddingSchemeDef::BlockPaddingScheme padding
        )
    {
        const unsigned char * cipherData = data;
        size_t cipherSize = dataLength;

        KeyType iv = this->BuildIV(data, dataLength, &cipherData, &cipherSize);
        m_decryptor.SetKeyWithIV(&m_key.at(0), m_key.size(), &iv.at(0));

        CryptoPP::StreamTransformationFilter filter(m_decryptor, NULL, padding);
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
