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

#ifndef __I_LCP_LINKS_H__
#define __I_LCP_LINKS_H__

#include <vector>
#include <string>
#include "LcpStatus.h"
#include "IValueIterator.h"

namespace lcp
{
    struct Link
    {
        Link()
        {
            templated = false;
            length = 0;
        }
        std::string href;
        std::string title;
        std::string type;
        std::string hash;
        bool templated;
        int length;
    };

    class ILinks
    {
    public:
        virtual IKeyValueIterator<std::string, Link> * Enumerate() const = 0;
        virtual bool Has(const std::string & name) const = 0;
        virtual bool GetLink(const std::string & name, Link & link) const = 0;
        virtual bool HasMany(const std::string & name) const = 0;
        virtual bool GetLinks(const std::string & name, std::vector<Link> & links) const = 0;
        virtual ~ILinks() {}
    };

    static const char * Publication = "publication";
    static const char * Hint = "hint";
    static const char * Self = "self";
}

#endif //__I_LCP_LINKS_H__
