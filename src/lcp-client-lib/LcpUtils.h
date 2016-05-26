// Copyright (c) 2016 Mantano, Readium.
// Licensed to the Readium Foundation under one or more contributor license agreements.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation and/or
//    other materials provided with the distribution.
// 3. Neither the name of the organization nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.



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