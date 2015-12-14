//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#ifndef __LCP_UTILS_H__
#define __LCP_UTILS_H__

#include <string>
#include "public/LcpStatus.h"

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

    bool StringToBool(const std::string & val);
    std::string BoolToString(bool val);
    std::string DoubleToExponentString(const double & value);

    void ValidateUtf8(const std::string & utf8Str);
    bool EqualsUtf8(const std::string & left, const std::string & right);
    bool LexicographicalCompareUtf8(const std::string & left, const std::string & right);
}

#endif //__LCP_UTILS_H__