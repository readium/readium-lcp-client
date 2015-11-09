#include <exception>
#include "LcpServiceCreator.h"
#include "LcpService.h"

namespace lcp
{
    Status LcpServiceCreator::CreateLcpService(ILcpService ** lcpService)
    {
        if (lcpService == nullptr)
        {
            throw std::invalid_argument("lcpService is nullptr");
        }
        auto status = Status(StCodeCover::ErrorCommonSuccess);
        *lcpService = new LcpService();
        return status;
    }
}