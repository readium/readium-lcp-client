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


#ifndef __I_ENCRTYPTION_PROFILE_H__
#define __I_ENCRTYPTION_PROFILE_H__

#include <string>
#include <vector>
#include "LcpTypedefs.h"

namespace lcp
{
    class ISymmetricAlgorithm;
    class IHashAlgorithm;
    class ISignatureAlgorithm;

    class IEncryptionProfile
    {
    public:
#if ENABLE_PROFILE_NAMES
        virtual std::string Name() const = 0;
#endif //ENABLE_PROFILE_NAMES
        virtual std::string UserKeyAlgorithm() const = 0;
        virtual std::string PublicationAlgorithmGCM() const = 0;
        virtual std::string PublicationAlgorithmCBC() const = 0;
        virtual std::string ContentKeyAlgorithmGCM() const = 0;
        virtual std::string ContentKeyAlgorithmCBC() const = 0;
        virtual std::string SignatureAlgorithmRSA() const = 0;
        virtual std::string SignatureAlgorithmECDSA() const = 0;
        virtual IHashAlgorithm * CreateUserKeyAlgorithm() const = 0;
        virtual ISymmetricAlgorithm * CreatePublicationAlgorithm(const KeyType & symmetricKey, const std::string & algorithm) const = 0;
        virtual ISymmetricAlgorithm * CreateContentKeyAlgorithm(const KeyType & symmetricKey, const std::string & algorithm) const = 0;
        virtual ISignatureAlgorithm * CreateSignatureAlgorithm(const KeyType & publicKey, const std::string & algorithm) const = 0;
        virtual ~IEncryptionProfile() {}
    };
}

#endif //__I_ENCRTYPTION_PROFILE_H__
