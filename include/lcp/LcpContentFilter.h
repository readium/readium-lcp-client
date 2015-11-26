//
//  Created by Mickaël Menu on 24/11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//

#ifndef __LCP_CONTENT_FILTER_H__
#define __LCP_CONTENT_FILTER_H__

#include "ILicense.h"
#include "ILcpService.h"
#include <ePub3/utilities/pointer_type.h>
#include <ePub3/ePub3.h>
#include <ePub3/filter.h>
#include <vector>

using namespace ePub3;

namespace lcp {
    class LcpContentFilter : public ContentFilter, public PointerType<LcpContentFilter>
    {
    public:
        LcpContentFilter(std::shared_ptr<ILicense> license) : ContentFilter(SniffLcpContent), m_license(license) {}
        LcpContentFilter(const LcpContentFilter &o) : ContentFilter(o) {}
        LcpContentFilter(LcpContentFilter &&o) : ContentFilter(std::move(o)) {}
        
        virtual void *FilterData(FilterContext *context, void *data, size_t len, size_t *outputLen) OVERRIDE;
        virtual OperatingMode GetOperatingMode() const OVERRIDE { return OperatingMode::RequiresCompleteData; }
    
        static void Register(std::shared_ptr<ILcpService> lcpService);
    
    protected:
        std::shared_ptr<ILicense> m_license;
        virtual FilterContext *InnerMakeFilterContext(ConstManifestItemPtr item) const OVERRIDE;
    
    private:
        static std::shared_ptr<ILcpService> lcpService;
        static bool SniffLcpContent(ConstManifestItemPtr item);
        static ContentFilterPtr Factory(ConstPackagePtr package);
    };
}

#endif // __LCP_CONTENT_FILTER_H__
