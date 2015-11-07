#ifndef __LCP_UTILS_H__
#define __LCP_UTILS_H__

#include "LcpStatus.h"

namespace lcp
{
    class StatusException : public std::runtime_error
    {
    public:
        StatusException(const Status & status)
            : std::runtime_error(status.Extension)
            , m_status(status)
        {
        }

        Status ResultStatus() const
        {
            return m_status;
        }

    private:
        Status m_status;
    };
}

#endif //__LCP_UTILS_H__