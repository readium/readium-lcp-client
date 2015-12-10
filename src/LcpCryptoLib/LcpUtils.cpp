//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//  Licensed under Gnu Affero General Public License Version 3 (provided, notwithstanding this notice,
//  Readium Foundation reserves the right to license this material under a different separate license,
//  and if you have done so, the terms of that separate license control and the following references
//  to GPL do not apply).
//
//  This program is free software: you can redistribute it and/or modify it under the terms of the GNU
//  Affero General Public License as published by the Free Software Foundation, either version 3 of
//  the License, or (at your option) any later version. You should have received a copy of the GNU
//  Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <sstream>
#include <iomanip>
#include "utf8-cpp/utf8.h"
#include "LcpUtils.h"

namespace lcp
{
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
        Utf8ConstIt leftBeginIt(left.begin(), left.begin(), left.end());
        Utf8ConstIt leftEndIt(left.end(), left.begin(), left.end());
        Utf8ConstIt rightBeginIt(right.begin(), right.begin(), right.end());
        
        return std::equal(leftBeginIt, leftEndIt, rightBeginIt);
    }

    bool LexicographicalCompareUtf8(const std::string & left, const std::string & right)
    {
        Utf8ConstIt leftBeginIt(left.begin(), left.begin(), left.end());
        Utf8ConstIt leftEndIt(left.end(), left.begin(), left.end());
        Utf8ConstIt rightBeginIt(right.begin(), right.begin(), right.end());
        Utf8ConstIt rightEndIt(right.end(), right.begin(), right.end());

        return std::lexicographical_compare(leftBeginIt, leftEndIt, rightBeginIt, rightEndIt);
    }
}