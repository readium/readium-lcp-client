//
//  Created by Mickaël Menu on 24/11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#ifndef __LCP_CONTENT_FILTER_H__
#define __LCP_CONTENT_FILTER_H__

#include "IncludeMacros.h"
#include "ILicense.h"
#include "ILcpService.h"

READIUM_INCLUDE_START
#include <ePub3/utilities/pointer_type.h>
//#include <ePub3/ePub3.h>
#include <ePub3/filter.h>
READIUM_INCLUDE_END

using namespace ePub3;
using namespace std;

namespace lcp {
    class LcpContentFilter : public ContentFilter, public PointerType<LcpContentFilter>
    {
    public:
        LcpContentFilter(ILicense *license) : ContentFilter(SniffLcpContent), m_license(license) {}
        LcpContentFilter(const LcpContentFilter &o) : ContentFilter(o) {}
        LcpContentFilter(LcpContentFilter &&o) : ContentFilter(move(o)) {}
        
        virtual void *FilterData(FilterContext *context, void *data, size_t len, size_t *outputLen) OVERRIDE;
        virtual OperatingMode GetOperatingMode() const OVERRIDE { return OperatingMode::SupportsByteRanges; }
        virtual ByteStream::size_type BytesAvailable(FilterContext *context, SeekableByteStream *byteStream) const OVERRIDE;
    
        static void Register(ILcpService *const lcpService);
    
    protected:
        ILicense *m_license;
        virtual FilterContext *InnerMakeFilterContext(ConstManifestItemPtr item) const OVERRIDE;
    
    private:
        static ILicense *license;
        static ILcpService *lcpService;

        static bool SniffLcpContent(ConstManifestItemPtr item);
        static ContentFilterPtr Factory(ConstPackagePtr package);
    };
}

#endif // __LCP_CONTENT_FILTER_H__
