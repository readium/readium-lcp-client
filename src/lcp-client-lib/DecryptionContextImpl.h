//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#ifndef __DECRYPTION_CONTEXT_IMPL_H__
#define __DECRYPTION_CONTEXT_IMPL_H__

#include "IDecryptionContext.h"

class DecryptionContextImpl : public lcp::IDecryptionContext
{
public:
    virtual lcp::DecRangeInfo GetDecryptionRange() const
    {
        return m_rangeInfo;
    }

    virtual void SetDecryptionRange(size_t position, size_t length)
    {
        m_rangeInfo.position = position;
        m_rangeInfo.length = length;
    }

private:
    lcp::DecRangeInfo m_rangeInfo;
};

#endif //__DECRYPTION_CONTEXT_IMPL_H__
