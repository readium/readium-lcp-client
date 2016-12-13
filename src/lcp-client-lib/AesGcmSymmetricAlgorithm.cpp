// Copyright (c) 2016 Mantano
// Licensed to the Readium Foundation under one or more contributor license agreements.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation and/or
//    other materials provided with the distribution.
// 3. Neither the name of the organization nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "AesGcmSymmetricAlgorithm.h"
#include "AlgorithmNames.h"
#include "CryptoppUtils.h"
#include "IDecryptionContext.h"
#include "public/StreamInterfaces.h"

namespace lcp
{
    AesGcmSymmetricAlgorithm::AesGcmSymmetricAlgorithm(
        const KeyType & key,
        KeySize keySize
        )
        : m_key(key)
        , m_keySize(keySize)
    {
        KeyType emptyIv(CryptoPP::AES::BLOCKSIZE);
        m_decryptor.SetKeyWithIV(&key.at(0), key.size(), &emptyIv.at(0));
    }

    std::string AesGcmSymmetricAlgorithm::Name() const
    {
        switch (m_keySize)
        {
        case lcp::AesGcmSymmetricAlgorithm::Key128:
            throw std::logic_error("not implemented");
        case lcp::AesGcmSymmetricAlgorithm::Key192:
            throw std::logic_error("not implemented");
        case lcp::AesGcmSymmetricAlgorithm::Key256:
            return AlgorithmNames::AesGcm256Id;
        default:
            throw std::logic_error("not implemented");
        }
    }

    std::string AesGcmSymmetricAlgorithm::Decrypt(
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
            new CryptoPP::AuthenticatedDecryptionFilter(m_decryptor,
                new CryptoPP::StringSink(decryptedDataStr)
                )
            );

        return decryptedDataStr;
    }

    size_t AesGcmSymmetricAlgorithm::Decrypt(
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

    size_t AesGcmSymmetricAlgorithm::PlainTextSize(IReadableStream * stream)
    {
        if (stream->Size() < CryptoPP::AES::BLOCKSIZE + CryptoPP::AES::BLOCKSIZE)
        {
            throw std::out_of_range("Invalid encrypted file, size is out of range");
        }

        size_t readPosition = static_cast<size_t>(stream->Size()) - (CryptoPP::AES::BLOCKSIZE + CryptoPP::AES::BLOCKSIZE);
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

        return static_cast<size_t>(stream->Size())
            - CryptoPP::AES::BLOCKSIZE // minus IV or previous block
            - (CryptoPP::AES::BLOCKSIZE - outSize) % CryptoPP::AES::BLOCKSIZE; // minus padding part
    }

    void AesGcmSymmetricAlgorithm::Decrypt(
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

    size_t AesGcmSymmetricAlgorithm::InnerDecrypt(
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

        CryptoPP::AuthenticatedDecryptionFilter filter(m_decryptor, NULL, padding);
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

    KeyType AesGcmSymmetricAlgorithm::BuildIV(
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
