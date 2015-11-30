//
//  Created by Mickaël Menu on 24/11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//

#include "LcpContentFilter.h"

#include "IDecryptionContext.h"
#include <ePub3/filter.h>
#include <vector>

namespace lcp {
    class LcpFilterContext : public RangeFilterContext
    {
    public:
        LcpFilterContext()
            : m_filteredBytesCount(0)
        {}
        
        virtual ~LcpFilterContext()
        {}
        
        virtual bool IsFirstRange() const
        {
            return m_filteredBytesCount == 0;
        }
        
        IDecryptionContext *DecryptionContext() const
        {
            return m_decryptionContext.get();
        }
        
        void SetDecryptionContext(IDecryptionContext *decryptionContext)
        {
            m_decryptionContext = std::unique_ptr<IDecryptionContext>(decryptionContext);
            m_decryptionContext->SetFirstRange(m_filteredBytesCount == 0);
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
            m_decryptionContext->SetFirstRange(m_filteredBytesCount == 0);
        }
        
    protected:
        std::unique_ptr<IDecryptionContext> m_decryptionContext;
        size_t m_filteredBytesCount;
        std::string m_algorithm;
    };
}
