#ifndef __I_STORAGE_PROVIDER_H__
#define __I_STORAGE_PROVIDER_H__

#include <string>
#include "Iterator.h"

namespace lcp
{
    class IStorageProvider
    {
    public:
        virtual void SetValue(const std::string & vaultId, const std::string & key, const std::string & value) = 0;
        virtual std::string GetValue(const std::string & vaultId, const std::string & key) = 0;
        virtual MapIterator<std::string> * EnumerateVault(const std::string & vaultId) = 0;
        virtual ~IStorageProvider() {}
    };

    static const char * UserKeysVaultId = "2b741732-f721-4182-9928-b9dcb7edb24e";
    static const char * LicenseRightsVaultId = "8cd95d47-ee95-4f09-b217-621352499d79";
}

#endif //__I_STORAGE_PROVIDER_H__
