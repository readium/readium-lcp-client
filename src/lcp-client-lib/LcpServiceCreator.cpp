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


#include <exception>
#include "public/LcpServiceCreator.h"
#include "LcpService.h"

namespace lcp
{
    //
    // Factory for creating LCP service instances.
    //
    Status LcpServiceCreator::CreateLcpService(
        const std::string & rootCertificate,
#if ENABLE_NET_PROVIDER
        INetProvider * netProvider,
#endif //ENABLE_NET_PROVIDER
        IStorageProvider * storageProvider,
        IFileSystemProvider * fileSystemProvider,
        ILcpService ** lcpService
#if ENABLE_NET_PROVIDER
            ,
        const std::string & defaultCrlUrl
#endif //ENABLE_NET_PROVIDER
        )
    {
        if (lcpService == nullptr)
        {
            throw std::invalid_argument("lcpService is nullptr");
        }
        auto status = Status(StatusCode::ErrorCommonSuccess);
        *lcpService = new LcpService(rootCertificate,
#if ENABLE_NET_PROVIDER
                                     netProvider,
#endif //ENABLE_NET_PROVIDER
                                     storageProvider, fileSystemProvider
#if ENABLE_NET_PROVIDER
                ,
                                     defaultCrlUrl
#endif //ENABLE_NET_PROVIDER
        );
        return status;
    }
}