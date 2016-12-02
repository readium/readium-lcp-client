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

#ifndef __I_STORAGE_PROVIDER_H__
#define __I_STORAGE_PROVIDER_H__

#include <string>
#include "IValueIterator.h"

namespace lcp
{
    //
    // Interface to be implemented by the client to handle the secure storage
    // of values on the device.
    // The storage must be encrypted and not be editable by the User. For
    // example, using the Keychain on iOS is a good implementation.
    // It is used in particular to store the User Keys and License rights
    // consumption.
    //
    // The storage provider must handle any number of vaults, which are secure
    // key-value maps.
    //
    class IStorageProvider
    {
    public:
        //
        // Gets or sets a value for the given key and vault identifier.
        //
        virtual std::string GetValue(const std::string & vaultId, const std::string & key) = 0;
        virtual void SetValue(const std::string & vaultId, const std::string & key, const std::string & value) = 0;

        //
        // Enumerates over all the key-values stored in the given vault
        // identifier.
        //
        virtual KvStringsIterator * EnumerateVault(const std::string & vaultId) = 0;

        virtual ~IStorageProvider() {}
    };

    //
    // Identifiers for the vaults used by the library. You can use them for
    // debugging purpose, or to flush their content if needed.
    //
    // Identifier for the vault storing User Keys.
    static const char * UserKeysVaultId = "2b741732-f721-4182-9928-b9dcb7edb24e";
    // Identifier for the vault storing the License rights consumption.
    static const char * LicenseRightsVaultId = "8cd95d47-ee95-4f09-b217-621352499d79";
}

#endif //__I_STORAGE_PROVIDER_H__
