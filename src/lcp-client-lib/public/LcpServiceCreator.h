//
//  Created by Mickaël Menu, Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#ifndef __LCP_SERVICE_CREATOR_H__
#define __LCP_SERVICE_CREATOR_H__

#include "LcpStatus.h"

namespace lcp
{
    class ILcpService;
    class INetProvider;
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
            INetProvider * netProvider,
            IStorageProvider * storageProvider,
            IFileSystemProvider * fileSystemProvider,
            ILcpService ** lcpService,
            const std::string & defaultCrlUrl = std::string()
            );
    };
}

#endif //__LCP_SERVICE_CREATOR_H__