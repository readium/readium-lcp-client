//
//  Created by Mickaël Menu on 24/11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//

#ifndef __LCP_FILTER_CONTEXT_H__
#define __LCP_FILTER_CONTEXT_H__

#include "LcpContentFilter.h"
#include <ePub3/filter.h>
#include <vector>

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
