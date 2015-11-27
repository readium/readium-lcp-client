#ifndef __LCP_SERVICE_CREATOR_H__
#define __LCP_SERVICE_CREATOR_H__

#include "LcpStatus.h"

namespace lcp
{
    class ILcpService;
    class INetProvider;
    class IStorageProvider;
    class IFileSystemProvider;

    class LcpServiceCreator
    {
    public:
        Status CreateLcpService(
            const std::string & rootCertificate,
            INetProvider * netProvider,
            IStorageProvider * storageProvider,
            IFileSystemProvider * fileSystemProvider,
            ILcpService ** lcpService
            );
    };
}

#endif //__LCP_SERVICE_CREATOR_H__