#ifndef __I_KEY_PROVIDER_H__
#define __I_KEY_PROVIDER_H__

#include "LcpUtils.h"

namespace lcp
{
    class IKeyProvider
    {
    public:
        virtual const KeyType & UserKey() const = 0;
        virtual const KeyType & ContentKey() const = 0;
        virtual ~IKeyProvider() {}
    };
}

#endif //__I_KEY_PROVIDER_H__
