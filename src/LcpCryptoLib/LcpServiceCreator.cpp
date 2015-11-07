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
        *lcpService = new LcpService();
        return Status(StCodeCover::ErrorSuccess);
    }
}