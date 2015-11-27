#ifndef __LINKS_LCP_NODE_H__
#define __LINKS_LCP_NODE_H__

#include <map>
#include <list>
#include "BaseLcpNode.h"
#include "Public/ILinks.h"

namespace lcp
{
    class LinksLcpNode : public BaseLcpNode, public ILinks
    {
    public:
        // ILcpNode
        virtual void ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader);

    public:
        // ILinks
        virtual IValueIterator<Link> * Enumerate() const;
        virtual bool Has(const std::string & name) const;
        virtual Link GetLink(const std::string & name) const;
        virtual bool HasMany(const std::string & name) const;
        virtual std::vector<Link> GetLinks(const std::string & name) const;

    private:
        Link ParseLinkValues(const rapidjson::Value & linkObject, JsonValueReader * reader);

    private:
        typedef std::multimap<std::string, Link> LinksMap;
        typedef LinksMap::const_iterator LinksMapConstIt;
        LinksMap m_linksMultiMap;
    };
}

#endif //__LINKS_LCP_NODE_H__
