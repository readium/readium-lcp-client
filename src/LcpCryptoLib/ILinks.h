#ifndef __I_LINKS_H__
#define __I_LINKS_H__

#include <vector>
#include <string>
#include "LcpStatus.h"

namespace lcp
{
    template<typename T> class IIterator;

    class ILinks
    {
    public:
        /*virtual Status Enumerate(IIterator<Link> ** iterator) = 0;
        virtual bool IsArray(const std::string & linkName) const = 0;
        virtual std::vector<std::string> LinkNames() const = 0;
        virtual Link * GetLinkByName(const std::string & linkName) = 0;
        virtual std::list<Link *> GetLinksByName(const std::string & linkName) = 0;*/
        virtual ~ILinks() {}

    public:
        static const char * PUBLICATION;
        static const char * HINT;
        static const char * SELF;
    };
}

#endif //__I_LINKS_H__
