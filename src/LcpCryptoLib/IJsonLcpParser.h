#ifndef __I_JSON_LCP_PARSER_H__
#define __I_JSON_LCP_PARSER_H__

#include <string>
#include "LcpStatus.h"

namespace lcp
{
    class ILicense;

    class IJsonLcpParser
    {
    public:
        virtual Status Parse(const std::string & licenseJson, ILicense ** license) = 0;
        virtual ~IJsonLcpParser() {}
    };
}

#endif //__I_JSON_LCP_PARSER_H__