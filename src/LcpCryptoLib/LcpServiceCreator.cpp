#include <exception>
#include "Public/LcpServiceCreator.h"
#include "LcpService.h"

namespace lcp
{
    Status LcpServiceCreator::CreateLcpService(
        const std::string & rootCertificate,
        INetProvider * netProvider,
        IStorageProvider * storageProvider,
        IFileSystemProvider * fileSystemProvider,
        ILcpService ** lcpService
        )
    {
        if (lcpService == nullptr)
        {
            throw std::invalid_argument("lcpService is nullptr");
        }
        auto status = Status(StCodeCover::ErrorCommonSuccess);
        *lcpService = new LcpService(rootCertificate, netProvider, storageProvider, fileSystemProvider);
        return status;
    }
}