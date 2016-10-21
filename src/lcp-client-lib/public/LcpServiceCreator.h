//
//  Created by Mickaël Menu, Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#ifndef __LCP_SERVICE_CREATOR_H__
#define __LCP_SERVICE_CREATOR_H__

#include "LcpStatus.h"

namespace lcp
{
    class ILcpService;
#if ENABLE_NET_PROVIDER
    class INetProvider;
#endif //ENABLE_NET_PROVIDER
    class IStorageProvider;
    class IFileSystemProvider;

    //
    // Factory used to create LCP service instances.
    //
    class LcpServiceCreator
    {
    public:
        Status CreateLcpService(
            const std::string & rootCertificate,
#if ENABLE_NET_PROVIDER
            INetProvider * netProvider,
#endif //ENABLE_NET_PROVIDER
            IStorageProvider * storageProvider,
            IFileSystemProvider * fileSystemProvider,
            ILcpService ** lcpService
#if ENABLE_NET_PROVIDER
                ,
            const std::string & defaultCrlUrl = std::string()
#endif //ENABLE_NET_PROVIDER
            );
    };
}

#endif //__LCP_SERVICE_CREATOR_H__