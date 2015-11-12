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
}