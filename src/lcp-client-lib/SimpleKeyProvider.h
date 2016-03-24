//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#ifndef __SIMPLE_KEY_PROVIDER_H__
#define __SIMPLE_KEY_PROVIDER_H__

#include "IKeyProvider.h"

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

        virtual KeyType UserKey() const
        {
            return m_userKey;
        }

        virtual KeyType ContentKey() const
        {
            return m_contentKey;
        }

    private:
        KeyType m_userKey;
        KeyType m_contentKey;
    };
}

#endif //__SIMPLE_KEY_PROVIDER_H__
