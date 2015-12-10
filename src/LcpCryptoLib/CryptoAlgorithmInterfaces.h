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

#ifndef __CRYPTO_ALGORITHM_INTERFACES_H__
#define __CRYPTO_ALGORITHM_INTERFACES_H__

#include <string>
#include <vector>
#include "LcpUtils.h"

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
