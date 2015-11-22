#ifndef __SIMPLE_KEY_PROVIDER_H__
#define __SIMPLE_KEY_PROVIDER_H__

#include "IKeyProvider.h"
#include "LcpUtils.h"

namespace lcp
{
    class SimpleKeyProvider : public IKeyProvider
    {
    public:
        SimpleKeyProvider(const KeyType & userKey, const KeyType & contentKey)
            : m_userKey(userKey)
            , m_contentKey(contentKey)
        {
        }

        virtual const KeyType & UserKey() const
        {
            return m_userKey;
        }

        virtual const KeyType & ContentKey() const
        {
            return m_contentKey;
        }

    private:
        KeyType m_userKey;
        KeyType m_contentKey;
    };
}

#endif //__SIMPLE_KEY_PROVIDER_H__
