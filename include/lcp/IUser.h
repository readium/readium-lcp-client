//
//  Created by Mickaël Menu, Artem Brazhnikov on 11/15.
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

#ifndef __I_LCP_USER_H__
#define __I_LCP_USER_H__

#include "IValueIterator.h"

namespace lcp
{
    class IUser
    {
    public:
        virtual std::string Id() const = 0;
        virtual std::string Email() const = 0;
        virtual std::string Name() const = 0;
        virtual bool HasUserValue(const std::string & name) const = 0;
        virtual bool GetUserValue(const std::string & name, std::string & value) const = 0;
        virtual KvStringsIterator * Enumerate() const = 0;
        virtual ~IUser() {}
    };
}

#endif //__I_LCP_USER_H__
