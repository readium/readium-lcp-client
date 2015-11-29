//
//  Created by Mickaël Menu on 24/11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//

#include "LcpContentFilter.h"

#include "DecryptionContextImpl.h"
#include <ePub3/filter.h>
#include <vector>

namespace lcp {
    class LcpFilterContext : public RangeFilterContext, public DecryptionContextImpl
    {
    public:
        LcpFilterContext()
            : m_filteredBytesCount(0)
        {}
        
        virtual ~LcpFilterContext() {}
        
        virtual bool IsFirstRange() const
        {
            return m_filteredBytesCount == 0;
        }
        
        std::string Algorithm() const
        {
            return m_algorithm;
        }
        
        void SetAlgorithm(const std::string algorithm)
        {
            m_algorithm = algorithm;
        }
        
        void AddFilteredBytesCount(size_t filteredBytesCount)
        {
            m_filteredBytesCount += filteredBytesCount;
        }
        
    protected:
        size_t m_filteredBytesCount;
        std::string m_algorithm;
    };
}
