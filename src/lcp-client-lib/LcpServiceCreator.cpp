//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#include <exception>
#include "Public/LcpServiceCreator.h"
#include "LcpService.h"

namespace lcp
{
    //
    // Factory for creating LCP service instances.
    //
    Status LcpServiceCreator::CreateLcpService(
        const std::string & rootCertificate,
        INetProvider * netProvider,
        IStorageProvider * storageProvider,
        IFileSystemProvider * fileSystemProvider,
        ILcpService ** lcpService,
        const std::string & defaultCrlUrl
        )
    {
        if (lcpService == nullptr)
        {
            throw std::invalid_argument("lcpService is nullptr");
        }
        auto status = Status(StatusCode::ErrorCommonSuccess);
        *lcpService = new LcpService(rootCertificate, netProvider, storageProvider, fileSystemProvider, defaultCrlUrl);
        return status;
    }
}