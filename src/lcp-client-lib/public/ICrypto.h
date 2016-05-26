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



#ifndef __I_LCP_CRYPTO_H__
#define __I_LCP_CRYPTO_H__

#include <string>

namespace lcp
{
    //
    // ICrypto contains the Encryption profile and info of a License Document.
    //
    class ICrypto
    {
    public:
        //
        // Encryption profile used by this LCP-protected publication,
        // as an URI (e.g. http://readium.org/lcp/profile-1.0)
        //
        virtual std::string EncryptionProfile() const = 0;

        //
        // Content Key encrypted with the User Key (base 64).
        //
        virtual std::string ContentKey() const = 0;

        //
        // Algorithm used to encrypt the Content Key, as an URI (W3C XML
        // Encryption).
        //
        virtual std::string ContentKeyAlgorithm() const = 0;

        //
        // Hint to be displayed to the User to help them remember the
        // passphrase.
        //
        virtual std::string UserKeyHint() const = 0;

        //
        // Algorithm used to generate the user key from the User 
        // Passphrase, as an URI (W3C XML Encryption).
        //
        virtual std::string UserKeyAlgorithm() const = 0;

        //
        // The value of the License's identifier field, encrypted using 
        // the User Key.
        //
        virtual std::string UserKeyCheck() const = 0;

        //
        // Value of the License signature.
        //
        virtual std::string Signature() const = 0;

        //
        // Algorithm used to calculate the signature, as an URI (W3C XML
        // Encryption).
        //
        virtual std::string SignatureAlgorithm() const = 0;

        //
        // X509 certificate used by the Content Provider.
        //
        virtual std::string SignatureCertificate() const = 0;

        virtual ~ICrypto() {}
    };
}

#endif //__I_LCP_CRYPTO_H__
