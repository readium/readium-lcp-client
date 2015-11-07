#ifndef __RAPID_JSON_LCP_PARSER_H__
#define __RAPID_JSON_LCP_PARSER_H__

#include "IJsonLcpParser.h"

namespace lcp
{
    class RapidJsonLcpParser : public IJsonLcpParser
    {
    public:
        virtual Status Parse(const std::string & licenseJson, ILicense ** license);
    };
}

#endif //__RAPID_JSON_LCP_PARSER_H__
