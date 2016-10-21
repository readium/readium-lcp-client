//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

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