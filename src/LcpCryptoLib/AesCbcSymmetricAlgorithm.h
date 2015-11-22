#ifndef __AES_CBC_SYMMETRIC_ALGORITHM_H__
#define __AES_CBC_SYMMETRIC_ALGORITHM_H__

#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include "CryptoAlgorithmInterfaces.h"

namespace lcp
{
    class AesCbcSymmetricAlgorithm : public ISymmetricAlgorithm
    {
    public:
        enum KeySize
        {
            Key128,
            Key192,
            Key256
        };

    public:
        explicit AesCbcSymmetricAlgorithm(
            const KeyType & key,
            KeySize keySize = Key256
            );

        virtual std::string Name() const;

        virtual std::string Decrypt(
            const std::string & encryptedDataBase64,
            bool containsIv = true
            );

        virtual size_t Decrypt(
            const unsigned char * data,
            size_t dataLength,
            unsigned char * decryptedData,
            size_t decryptedDataLength,
            bool containsIv = true
            );

    private:
        void BuildIV(
            const unsigned char * data,
            size_t dataLength,
            const unsigned char ** cipherData,
            size_t * cipherSize
            );

    private:
        KeySize m_keySize;
        KeyType m_key;
        KeyType m_iv;
        CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption m_decryptor;
    };
}

#endif //__AES_CBC_SYMMETRIC_ALGORITHM_H__
