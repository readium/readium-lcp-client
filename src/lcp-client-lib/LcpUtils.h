//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#ifndef __LCP_UTILS_H__
#define __LCP_UTILS_H__

#include <sstream>
#include <string>
#include <stdexcept>
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

    int StringToInt(const std::string & val);
    template <typename T>
    std::string ToString(T val) {
        std::stringstream stream;
        stream << val;
        return stream.str();
    }
}

#endif //__LCP_UTILS_H__