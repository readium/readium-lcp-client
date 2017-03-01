// Copyright (c) 2016 Mantano
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

#ifndef __I_ACQUISITION_H__
#define __I_ACQUISITION_H__

#if ENABLE_NET_PROVIDER_ACQUISITION

#include <string>
#include "LcpStatus.h"

namespace lcp
{
    class IAcquisitionCallback;

    //
    // The IAcquisition controls the download of a protected publication from
    // a standalone LCPL file. Once created using LcpService, it is idle. You
    // must start it and provide an implementation of IAcquisitionCallback
    // to be notified when the download progresses or ends.
    //
    // @see LcpService::CreatePublicationAcquisition()
    class IAcquisition
    {
    public:
        //
        // Starts an idle IAcquisition.
        //
        virtual Status Start(IAcquisitionCallback * callback) = 0;
        
        //
        // Cancels on on-going IAcquisition. The acquisition will be cancelled
        // as soon as it can, and the callback notified.
        //
        virtual Status Cancel() = 0;
        
        //
        // Absolute path to the downloaded publication, when the acquisition
        // ends.
        //
        virtual std::string PublicationPath() const = 0;
        
        //
        // Name suggested by the Content Provider server for the publication
        // file. You can use it to rename the downloaded publication, or
        // ignore it.
        //
        virtual std::string SuggestedFileName() const = 0;
        
        virtual ~IAcquisition() {}
    };
}

#endif //ENABLE_NET_PROVIDER_ACQUISITION

#endif //__I_ACQUISITION_H__
