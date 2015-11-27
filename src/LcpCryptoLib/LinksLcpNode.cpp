#include <algorithm>
#include "rapidjson/document.h"
#include "LinksLcpNode.h"
#include "JsonValueReader.h"
#include "LcpUtils.h"
#include "Public/ContainerIterator.h"

namespace lcp
{
    void LinksLcpNode::ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader)
    {
        const rapidjson::Value & linksObject = reader->ReadObjectCheck("links", parentObject);
        
        if (!linksObject.HasMember(Hint))
        {
            throw StatusException(Status(StCodeCover::ErrorOpeningLicenseNotValid, "links object is not valid"));
        }

        for (auto it = linksObject.MemberBegin(); it != linksObject.MemberEnd(); ++it)
        {
            rapidjson::Type type = it->value.GetType();

            if (type == rapidjson::kObjectType)
            {
                Link link = this->ParseLinkValues(it->value, reader);
                m_linksMultiMap.insert(std::make_pair(
                    std::string(it->name.GetString(), it->name.GetStringLength()), link)
                    );
            }
            else if (type == rapidjson::kArrayType)
            {
                for (auto arrayIt = it->value.Begin(); arrayIt != it->value.End(); ++arrayIt)
                {
                    Link link = this->ParseLinkValues(*arrayIt, reader);
                    m_linksMultiMap.insert(std::make_pair(
                        std::string(it->name.GetString(), it->name.GetStringLength()), link)
                        );
                }
            }
            else
            {
                throw StatusException(Status(StCodeCover::ErrorOpeningLicenseNotValid, "links object is not valid"));
            }
        }

        BaseLcpNode::ParseNode(linksObject, reader);
    }

    std::vector<Link> LinksLcpNode::GetLinks(const std::string & name) const
    {
        std::vector<Link> result;
        std::pair<LinksMapConstIt, LinksMapConstIt> range = m_linksMultiMap.equal_range(name);
        std::transform(
            range.first, range.second,
            std::back_inserter(result),
            [&result](const LinksMap::value_type & val) { return val.second; }
        );
        return result;
    }

    Link LinksLcpNode::GetLink(const std::string & name) const
    {
        auto it = m_linksMultiMap.find(name);
        if (it != m_linksMultiMap.end())
        {
            return it->second;
        }
        else
        {
            throw std::runtime_error("Can not find element with name: " + name);
        }
    }

    bool LinksLcpNode::HasMany(const std::string & name) const
    {
        auto range = m_linksMultiMap.equal_range(name);
        return std::distance(range.first, range.second) > 1;
    }

    bool LinksLcpNode::Has(const std::string & name) const
    {
        return (m_linksMultiMap.find(name) != m_linksMultiMap.end());
    }

    IValueIterator<Link> * LinksLcpNode::Enumerate() const
    {
        return new MultiMapIterator<Link>(m_linksMultiMap);
    }

    Link LinksLcpNode::ParseLinkValues(const rapidjson::Value & linkObject, JsonValueReader * reader)
    {
        Link link;
        link.href = reader->ReadStringCheck("href", linkObject);
        link.title = reader->ReadString("title", linkObject);
        link.type = reader->ReadString("type", linkObject);
        link.templated = reader->ReadBoolean("templated", linkObject);
        link.length = reader->ReadInteger("length", linkObject);
        link.hash = reader->ReadString("hash", linkObject);
        return link;
    }
}