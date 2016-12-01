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


#ifndef __CRYPTO_ALGORITHM_INTERFACES_H__
#define __CRYPTO_ALGORITHM_INTERFACES_H__

#include <string>
#include <vector>
#include "LcpTypedefs.h"

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
