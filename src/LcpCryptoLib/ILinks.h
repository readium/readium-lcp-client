#ifndef __I_LCP_LINKS_H__
#define __I_LCP_LINKS_H__

#include <vector>
#include <string>
#include "LcpStatus.h"

namespace lcp
{
    struct Link
    {
        std::string href;
        std::string title;
        std::string type;
        std::string templated;
        std::string length;
        std::string hash;
    };

    class ILinks
    {
    public:
        virtual std::vector<Link> GetLinks(const std::string & name) const = 0;
        //virtual Status Enumerate(IIterator<Link> ** iterator) const = 0;
        //virtual bool IsArray(const std::string & name) const = 0;
        virtual ~ILinks() {}

    public:
        static const char * PUBLICATION;
        static const char * HINT;
        static const char * SELF;
    };
}

#endif //__I_LCP_LINKS_H__
