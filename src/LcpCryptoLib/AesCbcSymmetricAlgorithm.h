//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//  Licensed under Gnu Affero General Public License Version 3 (provided, notwithstanding this notice,
//  Readium Foundation reserves the right to license this material under a different separate license,
//  and if you have done so, the terms of that separate license control and the following references
//  to GPL do not apply).
//
//  This program is free software: you can redistribute it and/or modify it under the terms of the GNU
//  Affero General Public License as published by the Free Software Foundation, either version 3 of
//  the License, or (at your option) any later version. You should have received a copy of the GNU
//  Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef __AES_CBC_SYMMETRIC_ALGORITHM_H__
#define __AES_CBC_SYMMETRIC_ALGORITHM_H__

#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
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
            const std::string & encryptedDataBase64
            );

        virtual size_t Decrypt(
            const unsigned char * data,
            size_t dataLength,
            unsigned char * decryptedData,
            size_t decryptedDataLength
            );

        virtual void Decrypt(
            IDecryptionContext * context,
            IReadableStream * stream,
            unsigned char * decryptedData,
            size_t decryptedDataLength
            );

        size_t PlainTextSize(IReadableStream * stream);

    private:
        size_t InnerDecrypt(
            const unsigned char * data,
            size_t dataLength,
            unsigned char * decryptedData,
            size_t decryptedDataLength,
            CryptoPP::BlockPaddingSchemeDef::BlockPaddingScheme padding
            );

        KeyType BuildIV(
            const unsigned char * data,
            size_t dataLength,
            const unsigned char ** cipherData,
            size_t * cipherSize
            );

    private:
        KeySize m_keySize;
        KeyType m_key;
        CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption m_decryptor;
    };
}

#endif //__AES_CBC_SYMMETRIC_ALGORITHM_H__
