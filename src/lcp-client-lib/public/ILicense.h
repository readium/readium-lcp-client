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

#ifndef __I_LCP_LICENSE__
#define __I_LCP_LICENSE__

#include <string>

namespace lcp
{
    class ICrypto;
    class ILinks;
    class IUser;
    class IRights;

    //
    // Represents a single License Document, and provides access to its
    // metadata.
    //
    class ILicense
    {
    public:
        //
        // Unique identifier for this license
        //
        virtual std::string Id() const = 0;

        //
        // Canonical JSON form of the license. Used when validating the signature.
        //
        virtual std::string CanonicalContent() const = 0;

        //
        // Original JSON form of the license, given to the LcpService when
        // opening the License.
        //
        virtual std::string OriginalContent() const = 0;

        //
        // Date when the license was first issued (ISO 8601).
        //
        virtual std::string Issued() const = 0;

        //
        // Date when the license was last updated (ISO 8601).
        //
        virtual std::string Updated() const = 0;

        //
        // Unique identifier for the Content Provider, as an URI.
        //
        virtual std::string Provider() const = 0;

        //
        // Encryption Profile metadata for this License.
        //
        virtual ICrypto * Crypto() const = 0;

        //
        // List of links available in the License.
        //
        virtual ILinks * Links() const = 0;

        //
        // Information about the User.
        //
        virtual IUser * User() const = 0;

        //
        // Current values (static or stored) for the License rights attributed
        // to the User. The values will be automatically synchronized with the
        // IStorageProvider to match the consumption, if relevant (eg. copy count).
        //
        virtual IRights * Rights() const = 0;

        //
        // Returns true if the License is decrypted (ie. the passphrase
        // was given to the LcpService.
        //
        virtual bool Decrypted() const = 0;

        virtual ~ILicense() {};

        virtual bool getStatusDocumentProcessingFlag() const = 0;
        virtual void setStatusDocumentProcessingFlag(bool flag) = 0;
    };
}

#endif //__I_LCP_LICENSE__
