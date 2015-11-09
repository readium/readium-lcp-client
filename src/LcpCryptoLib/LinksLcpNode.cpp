#include "rapidjson/document.h"
#include "LinksLcpNode.h"
#include "JsonValueReader.h"


namespace lcp
{
    Status LinksLcpNode::ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader)
    {
        const rapidjson::Value & linksObject = reader->ReadAsObjectCheck("links", parentObject);
        
        if (!linksObject.HasMember("hint") ||
            !linksObject.HasMember("publication") ||
            !linksObject.HasMember("self"))
        {
            return Status(StCodeCover::ErrorLicenseNotValid, "links object is not valid");
        }

        for (auto it = linksObject.MemberBegin(); it != linksObject.MemberEnd(); ++it)
        {
            rapidjson::Type type = it->value.GetType();

            if (type = rapidjson::kObjectType)
            {
                Link link = this->ParseLinkValues(it->value, reader);
                m_linksMap.insert(std::make_pair(it->name.GetString(), link));
            }
            else if (type == rapidjson::kArrayType)
            {
                std::list<Link> linksList;
                for (auto arrayIt = it->value.Begin(); arrayIt != it->value.End(); ++arrayIt)
                {
                    Link link = this->ParseLinkValues(it->value, reader);
                    linksList.push_back(std::move(link));
                }
                m_linksListMap.insert(std::make_pair(
                    it->name.GetString(), std::move(linksList))
                    );
            }
            else
            {
                return Status(StCodeCover::ErrorLicenseNotValid, "links object is not valid");
            }
        }
        return Status(StCodeCover::ErrorSuccess);
    }

    Link LinksLcpNode::ParseLinkValues(const rapidjson::Value & linkObject, JsonValueReader * reader)
    {
        Link link;
        link.href = reader->ReadAsStringCheck("href", linkObject);
        link.title = reader->ReadAsString("title", linkObject);
        link.type = reader->ReadAsString("type", linkObject);
        link.templated = reader->ReadAsString("templated", linkObject);
        link.length = reader->ReadAsString("length", linkObject);
        link.hash = reader->ReadAsString("hash", linkObject);
        return link;
    }
}