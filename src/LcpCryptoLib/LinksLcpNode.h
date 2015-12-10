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

#ifndef __LINKS_LCP_NODE_H__
#define __LINKS_LCP_NODE_H__

#include <map>
#include <list>
#include "BaseLcpNode.h"
#include "Public/ILinks.h"

namespace lcp
{
    class LinksLcpNode : public BaseLcpNode, public ILinks
    {
    public:
        // ILcpNode
        virtual void ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader);

    public:
        // ILinks
        virtual IKeyValueIterator<std::string, Link> * Enumerate() const;
        virtual bool Has(const std::string & name) const;
        virtual bool GetLink(const std::string & name, Link & link) const;
        virtual bool HasMany(const std::string & name) const;
        virtual bool GetLinks(const std::string & name, std::vector<Link> & links) const;

    private:
        Link ParseLinkValues(const rapidjson::Value & linkObject, JsonValueReader * reader);

    private:
        typedef std::multimap<std::string, Link> LinksMap;
        typedef LinksMap::const_iterator LinksMapConstIt;
        LinksMap m_linksMultiMap;
    };
}

#endif //__LINKS_LCP_NODE_H__
