//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//  Licensed under Gnu Affero General Public License Version 3 (provided, notwithstanding this notice,
//  Readium Foundation reserves the right to license this material under a different separate license,
//  and if you have done so, the terms of that separate license control and the following references
//  to GPL do not apply).
//
//  This program is free software: you can redistribute it and/or modify it under the terms of the GNU
//  Affero General Public License as published by the Free Software Foundation, either version 3 of
//  the License, or (at your option) any later version. You should have received a copy of the GNU
//  Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

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
