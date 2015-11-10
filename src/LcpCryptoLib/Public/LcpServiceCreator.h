#ifndef __LCP_SERVICE_CREATOR_H__
#define __LCP_SERVICE_CREATOR_H__

#include "LcpStatus.h"

namespace lcp
{
    class ILcpService;

    class LcpServiceCreator
    {
    public:
        Status CreateLcpService(ILcpService ** lcpService);
    };
}

#endif //__LCP_SERVICE_CREATOR_H__