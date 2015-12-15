//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
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
