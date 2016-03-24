//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#ifndef __NON_COPYABLE_H__
#define __NON_COPYABLE_H__

namespace lcp
{
    class NonCopyable
    {
    public:
        NonCopyable() = default;
        NonCopyable(const NonCopyable &) = delete;
        NonCopyable & operator=(const NonCopyable &) = delete;
    };
}

#endif //__NON_COPYABLE_H__
