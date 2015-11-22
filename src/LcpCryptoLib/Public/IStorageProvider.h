#ifndef __I_STORAGE_PROVIDER_H__
#define __I_STORAGE_PROVIDER_H__

#include <string>

namespace lcp
{
    class IStorageProvider
    {
    public:
        virtual std::string Get(const std::string & vaultId, const std::string & key) = 0;
        virtual void Set(const std::string & vaultId, const std::string & key, const std::string & value) = 0;
        virtual ~IStorageProvider() {}
    };
}

#endif //__I_STORAGE_PROVIDER_H__
