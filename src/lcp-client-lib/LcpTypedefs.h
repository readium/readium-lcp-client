//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#ifndef __LCP_TYPEDEFS_H__
#define __LCP_TYPEDEFS_H__

#include <string>
#include <map>
#include <vector>
#include <set>
#include <list>

namespace lcp
{
    typedef std::map<std::string, std::string> StringsMap;
    typedef std::vector<unsigned char> KeyType;
    typedef std::set<std::string> StringsSet;
    typedef std::list<std::string> StringsList;
    typedef std::vector<unsigned char> Buffer;
}

#endif //__LCP_TYPEDEFS_H__
