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

#ifndef __I_LCP_USER_H__
#define __I_LCP_USER_H__

#include "IValueIterator.h"

namespace lcp
{
    //
    // Metadata about the User of a License document.
    //
    class IUser
    {
    public:
        //
        // Unique identifier of the User for the Content Provider.
        //
        virtual std::string Id() const = 0;

        //
        // E-mail address of the user.
        //
        virtual std::string Email() const = 0;

        //
        // Name of the user.
        //
        virtual std::string Name() const = 0;

        //
        // Returns whether the License has the given user info extension.
        //
        virtual bool HasUserValue(const std::string & name) const = 0;

        //
        // Gets the value of the given user extension name.
        // Returns whether the value could be found.
        //
        virtual bool GetUserValue(const std::string & name, std::string & value) const = 0;

        //
        // Enumerates over the User metadata available in the License.
        //
        virtual KvStringsIterator * Enumerate() const = 0;
        
        virtual ~IUser() {}
    };
}

#endif //__I_LCP_USER_H__
