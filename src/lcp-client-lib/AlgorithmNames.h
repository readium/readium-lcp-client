//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#ifndef __ALGORITHM_NAMES_H__
#define __ALGORITHM_NAMES_H__

#include <string>

namespace lcp
{
    class AlgorithmNames
    {
    public:
        static std::string AesCbc256Id;
        static std::string Sha256Id;
        static std::string RsaSha256Id;
    };
}

#endif //__ALGORITHM_NAMES_H__
