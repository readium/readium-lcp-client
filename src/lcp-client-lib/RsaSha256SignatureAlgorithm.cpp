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


#include "RsaSha256SignatureAlgorithm.h"
#include "AlgorithmNames.h"
#include "CryptoppUtils.h"

namespace lcp
{
    RsaSha256SignatureAlgorithm::RsaSha256SignatureAlgorithm(const KeyType & publicKey)
    {
        ByteQueue publicKeyQueue;
        KeyType rawPublicKey = publicKey;
        publicKeyQueue.Put(&rawPublicKey.at(0), rawPublicKey.size());
        publicKeyQueue.MessageEnd();
        m_publicKey.BERDecode(publicKeyQueue);
    }

    std::string RsaSha256SignatureAlgorithm::Name() const
    {
        return AlgorithmNames::RsaSha256Id;
    }

    bool RsaSha256SignatureAlgorithm::VerifySignature(
        const std::string & message,
        const std::string & signatureBase64
        )
    {
        CryptoPP::SecByteBlock rawSignature;
        CryptoppUtils::Base64ToSecBlock(signatureBase64, rawSignature);

        return this->VerifySignature(
            reinterpret_cast<const byte *>(message.data()),
            message.size(),
            rawSignature.data(),
            rawSignature.size()
            );
    }

    bool RsaSha256SignatureAlgorithm::VerifySignature(
        const unsigned char * message,
        size_t messageLength,
        const unsigned char * signature,
        size_t signatureLength
        )
    {
        ThisVerifier verifier(m_publicKey);
        return verifier.VerifyMessage(
            message,
            messageLength,
            signature,
            signatureLength
            );
    }
}