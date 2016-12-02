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


#ifndef __RSA_SHA256_SIGNATURE_ALGORITHM_H__
#define __RSA_SHA256_SIGNATURE_ALGORITHM_H__

#include "CryptoAlgorithmInterfaces.h"
#include "IncludeMacros.h"
#include "NonCopyable.h"

CRYPTOPP_INCLUDE_START
#include <cryptopp/rsa.h>
CRYPTOPP_INCLUDE_END

namespace lcp
{
    class RsaSha256SignatureAlgorithm : public ISignatureAlgorithm, public NonCopyable
    {
    public:
        explicit RsaSha256SignatureAlgorithm(const KeyType & publicKey);

        virtual std::string Name() const;

        virtual bool VerifySignature(
            const std::string & message,
            const std::string & signatureBase64
            );

        virtual bool VerifySignature(
            const unsigned char * message,
            size_t messageLength,
            const unsigned char * signature,
            size_t signatureLength
            );

    private:
        typedef CryptoPP::RSASS<CryptoPP::PKCS1v15, CryptoPP::SHA256>::Verifier ThisVerifier;
        CryptoPP::RSA::PublicKey m_publicKey;
    };
}

#endif //__RSA_SHA256_SIGNATURE_ALGORITHM_H__
