// Copyright (c) 2016 Mantano
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


#include <iomanip>
#include <limits>
#include "utf8-cpp/utf8.h"
#include "LcpUtils.h"

namespace lcp
{
    int StringToInt(const std::string & val) {
        std::stringstream ss(val);
        int result;
        ss<<val;
        ss>>result;
        return result;
    }

    std::string BoolToString(bool val)
    {
        return (val) ? "true" : "false";
    }

    bool StringToBool(const std::string & val)
    {
        return (val == "true") ? true : false;
    }

    //TODO: for tests
    //"0e-15";
    //".0e-15";
    //"1.e-15";
    //"1.0e-15"
    //"1.00000000e-15"
    //"3.14E+00"
    //"2.4894324894e-5"

    std::string DoubleToExponentString(const double & value)
    {
        std::stringstream strm;
        strm << std::setprecision(std::numeric_limits<long double>::digits10)
            << std::scientific << std::uppercase << value;

        std::string doubleStr = strm.str();
        size_t expPos = doubleStr.find('E');
        std::string mantissa = doubleStr.substr(0, expPos);
        std::string exponent = doubleStr.substr(expPos);

        auto endIt = mantissa.end();
        for (auto it = mantissa.rbegin(); it != mantissa.rend(); ++it)
        {
            if (*it == '0')
                endIt = --(it.base());
            else if (*it == '.' && endIt != mantissa.end())
                ++endIt;
            else
                break;
        }
        if (endIt != mantissa.end())
        {
            mantissa.assign(mantissa.begin(), endIt);
        }

        doubleStr = std::move(mantissa + exponent);
        ValidateUtf8(doubleStr);
        return doubleStr;
    }

    void ValidateUtf8(const std::string & utf8Str)
    {
        auto endIt = utf8::find_invalid(utf8Str.begin(), utf8Str.end());
        if (endIt != utf8Str.end())
        {
            throw utf8::invalid_utf8(*endIt);
        }
    }

    typedef utf8::iterator<std::string::const_iterator> Utf8ConstIt;

    bool EqualsUtf8(const std::string & left, const std::string & right)
    {
        ValidateUtf8(left);
        ValidateUtf8(right);
        
        Utf8ConstIt leftBeginIt(left.begin(), left.begin(), left.end());
        Utf8ConstIt leftEndIt(left.end(), left.begin(), left.end());
        Utf8ConstIt rightBeginIt(right.begin(), right.begin(), right.end());
        
        return std::equal(leftBeginIt, leftEndIt, rightBeginIt);
    }

    bool LexicographicalCompareUtf8(const std::string & left, const std::string & right)
    {
        ValidateUtf8(left);
        ValidateUtf8(right);
        
        Utf8ConstIt leftBeginIt(left.begin(), left.begin(), left.end());
        Utf8ConstIt leftEndIt(left.end(), left.begin(), left.end());
        Utf8ConstIt rightBeginIt(right.begin(), right.begin(), right.end());
        Utf8ConstIt rightEndIt(right.end(), right.begin(), right.end());

        return std::lexicographical_compare(leftBeginIt, leftEndIt, rightBeginIt, rightEndIt);
    }
}
