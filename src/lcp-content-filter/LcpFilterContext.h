//
//  Created by Mickaël Menu on 24/11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#ifndef __LCP_FILTER_CONTEXT_H__
#define __LCP_FILTER_CONTEXT_H__

#include "IncludeMacros.h"
#include "LcpContentFilter.h"
#include <vector>

READIUM_INCLUDE_START
#include <ePub3/filter.h>
READIUM_INCLUDE_END

namespace lcp {
    class LcpFilterContext : public RangeFilterContext
    {
    public:
        std::string Algorithm() const
        {
            return m_algorithm;
        }
        
        void SetAlgorithm(const std::string algorithm)
        {
            m_algorithm = algorithm;
        }
        
    protected:
        std::string m_algorithm;
    };
}

#endif // __LCP_FILTER_CONTEXT_H__
