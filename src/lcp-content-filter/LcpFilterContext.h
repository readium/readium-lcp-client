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

        const ePub3::string& CompressionMethod() const
        {
            return m_compressionMethod;
        }

        const ePub3::string& OriginalLength() const
        {
            return m_originalLength;
        }

        void SetAlgorithm(const std::string algorithm)
        {
            m_algorithm = algorithm;
        }

        void SetCompressionInfo(const ePub3::string& compressionMethod, const ePub3::string& originalLength)
        {
            m_compressionMethod = compressionMethod;
            m_originalLength = originalLength;
        }


    protected:
        std::string m_algorithm;
        ePub3::string m_compressionMethod;
        ePub3::string m_originalLength;
    };
}

#endif // __LCP_FILTER_CONTEXT_H__
