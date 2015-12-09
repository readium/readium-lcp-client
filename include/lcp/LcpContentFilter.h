//
//  Created by Mickaël Menu on 24/11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//  Licensed under Gnu Affero General Public License Version 3 (provided, notwithstanding this notice,
//  Readium Foundation reserves the right to license this material under a different separate license,
//  and if you have done so, the terms of that separate license control and the following references
//  to GPL do not apply).
//
//  This program is free software: you can redistribute it and/or modify it under the terms of the GNU
//  Affero General Public License as published by the Free Software Foundation, either version 3 of
//  the License, or (at your option) any later version. You should have received a copy of the GNU
//  Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef __LCP_CONTENT_FILTER_H__
#define __LCP_CONTENT_FILTER_H__

#include "ILicense.h"
#include "ILcpService.h"
#include <ePub3/utilities/pointer_type.h>
#include <ePub3/ePub3.h>
#include <ePub3/filter.h>

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
        static ILcpService *lcpService;
        static bool SniffLcpContent(ConstManifestItemPtr item);
        static ContentFilterPtr Factory(ConstPackagePtr package);
    };
}

#endif // __LCP_CONTENT_FILTER_H__
