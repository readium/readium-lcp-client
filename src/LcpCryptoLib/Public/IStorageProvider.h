//
//  Created by Mickaël Menu, Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

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
