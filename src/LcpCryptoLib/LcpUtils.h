#ifndef __LCP_UTILS_H__
#define __LCP_UTILS_H__

#include "LcpStatus.h"

namespace lcp
{
    Status CreateStatus(StatusCover::StatusCode statusCode, const char * extension = "")
    {
        Status res;
        res.ResultCode = statusCode;
        res.Extension = extension;
        return res;
    }
}

#endif //__LCP_UTILS_H__