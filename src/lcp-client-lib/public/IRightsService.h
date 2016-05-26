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



#ifndef __I_RIGHTS_SERVICE_H__
#define __I_RIGHTS_SERVICE_H__

#include <string>

namespace lcp
{
    class ILicense;

    //
    // Service used to handle the use of License rights attributed to the User.
    // The IStorageProvider is used to store the rights consumption securely.
    //
    // The (Can)UseRight() methods provides an easy way to consume and check
    // the official rights defined in the LCP specification (eg. copy, start).
    // For custom right extensions, clients can use the methods (Set|Get)Value()
    // directly if they understand the extension's value format.
    // 
    class IRightsService
    {
    public:
        //
        // Returns whether the user can use the given right. Only works with
        // the official rights defined in the specification.
        // For example, "copy" returns whether the User can still copy text
        // from the publication.
        // For "end", returns whether the License isn't expired yet.
        // If the right identifier can't be found in the License, returns true
        // as all rights are unrestrained by default.
        //
        virtual bool CanUseRight(ILicense * license, const std::string & rightId) const = 0;

        //
        // Consumes one or more right value units. Only works with official
        // rights with a set amount, eg. copy and print. If no amount is given,
        // assume 1 unit. If the remaining units count is not enough for this
        // amount, it will return false and will not decrement the count.
        //
        virtual bool UseRight(ILicense * license, const std::string & rightId) = 0;
        virtual bool UseRight(ILicense * license, const std::string & rightId, int amount) = 0;

        //
        // Stores and gets the given value string for the right identifier.
        // It can be used by clients to implement custom right extension.
        //
        virtual void SetValue(ILicense * license, const std::string & rightId, const std::string & value) = 0;
        virtual std::string GetValue(ILicense * license, const std::string & rightId) const = 0;

        virtual ~IRightsService() {}
        
        //
        // Special value used for rights with a set amount (eg. copy and print)
        // when its use is unlimited.
        //
        static int UNLIMITED;
    };
}

#endif //__I_RIGHTS_SERVICE_H__
