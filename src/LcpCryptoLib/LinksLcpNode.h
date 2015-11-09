#ifndef __LINKS_LCP_NODE_H__
#define __LINKS_LCP_NODE_H__

#include <map>
#include <list>
#include "BaseLcpNode.h"
#include "ILinks.h"

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

    class LinksLcpNode : public BaseLcpNode, public ILinks
    {
    public:
        virtual Status ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader);

    private:
        Link ParseLinkValues(const rapidjson::Value & linkObject, JsonValueReader * reader);

    private:
        std::multimap<std::string, Link> m_linksMultiMap;
        std::map<std::string, Link> m_linksMap;
        std::map<std::string, std::list<Link> > m_linksListMap;
    };
}

#endif //__LINKS_LCP_NODE_H__
