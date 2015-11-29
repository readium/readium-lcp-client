#ifndef __I_KEY_PROVIDER_H__
#define __I_KEY_PROVIDER_H__

#include "LcpUtils.h"

namespace lcp
{
    class IKeyProvider
    {
    public:
        virtual KeyType UserKey() const = 0;
        virtual KeyType ContentKey() const = 0;
        virtual ~IKeyProvider() {}
    };
}

#endif //__I_KEY_PROVIDER_H__
