//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#ifndef __I_DECRYPTION_CONTEXT_H__
#define __I_DECRYPTION_CONTEXT_H__

#include <stdlib.h>

namespace lcp
{
    struct DecRangeInfo
    {
        DecRangeInfo()
            : position(0)
            , length(0)
        {}
        size_t position;
        size_t length;
    };

    class IDecryptionContext
    {
    public:
        virtual DecRangeInfo GetDecryptionRange() const = 0;
        virtual ~IDecryptionContext() {}
    };
}

#endif //__I_DECRYPTION_CONTEXT_H__
