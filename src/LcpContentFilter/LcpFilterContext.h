//
//  Created by Mickaël Menu on 24/11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//

#include "LcpContentFilter.h"

#include "IDecryptionContext.h"
#include <ePub3/filter.h>
#include <vector>

namespace lcp {
    class LcpFilterContext : public FilterContext, public IDecryptionContext
    {
    public:
        LcpFilterContext()
            : m_firstPart(false)
        {
        }
        
        virtual ~LcpFilterContext() {}
        
        virtual bool IsFirstRange() const
        {
            return m_firstPart;
        }
        
        virtual void SetFirstRange(bool value)
        {
            m_firstPart = value;
        }
        
        virtual bool HasIV() const
        {
            return !m_iv.empty();
        }
        
        virtual void SetIV(const std::vector<unsigned char> & iv)
        {
            m_iv = iv;
        }
        
        virtual std::vector<unsigned char> IV() const
        {
            return m_iv;
        }
        
        virtual std::string Algorithm() const
        {
            return m_algorithm;
        }
        
        virtual void SetAlgorithm(const std::string algorithm)
        {
            m_algorithm = algorithm;
        }
        
    protected:
        bool m_firstPart;
        std::vector<unsigned char> m_iv;
        std::string m_algorithm;
    };
}
