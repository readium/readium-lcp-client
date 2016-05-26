// Copyright (c) 2016 Mantano, Readium.
// Licensed to the Readium Foundation under one or more contributor license agreements.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation and/or
//    other materials provided with the distribution.
// 3. Neither the name of the organization nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.



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
        static ILcpService *lcpService;
        static bool SniffLcpContent(ConstManifestItemPtr item);
        static ContentFilterPtr Factory(ConstPackagePtr package);
    };
}

#endif // __LCP_CONTENT_FILTER_H__
