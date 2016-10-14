//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#include <algorithm>
#include "rapidjson/document.h"
#include "LinksLcpNode.h"
#include "JsonValueReader.h"
#include "LcpUtils.h"
#include "ContainerIterator.h"

namespace lcp
{
    void LinksLcpNode::ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader)
    {
        auto ite = parentObject.FindMember("links");
        if (ite == parentObject.MemberEnd())
        {
            throw StatusException(Status(StatusCode::ErrorOpeningLicenseNotValid, "links object is not valid"));
        }

        // ite->name == "links"

        // legacy JSON format, links is object map (now array of objects with "rel" field instead of object key)
        if (ite->value.IsObject())
        {
            const rapidjson::Value & linksObject = ite->value; //reader->ReadObjectCheck("links", parentObject);
            
            if (!linksObject.HasMember(Hint))
            {
                throw StatusException(Status(StatusCode::ErrorOpeningLicenseNotValid, "links object is not valid"));
            }

            for (auto it = linksObject.MemberBegin(); it != linksObject.MemberEnd(); ++it)
            {
                auto name = std::string(it->name.GetString(), it->name.GetStringLength());

                rapidjson::Type type = it->value.GetType();

                if (type == rapidjson::kObjectType)
                {
                    Link link = this->ParseLinkValues(it->value, reader);
                    m_linksMultiMap.insert(std::make_pair(name, link));
                }
                else if (type == rapidjson::kArrayType)
                {
                    for (auto arrayIt = it->value.Begin(); arrayIt != it->value.End(); ++arrayIt)
                    {
                        Link link = this->ParseLinkValues(*arrayIt, reader);
                        m_linksMultiMap.insert(std::make_pair(name, link));
                    }
                }
                else
                {
                    throw StatusException(Status(StatusCode::ErrorOpeningLicenseNotValid, "links object is not valid"));
                }
            }

            // does nothing (links objects have no children)
            // see LcpService.spp rootNode->AddChildNode(std::move(linksNode))
            BaseLcpNode::ParseNode(linksObject, reader);
        }
        else if (ite->value.IsArray())
        {
            const rapidjson::Value & linksArray = ite->value;

            bool hintFound = false;

            for (auto arrayIt = linksArray.Begin(); arrayIt != linksArray.End(); ++arrayIt)
            {
                if (!arrayIt->IsObject())
                {
                    throw StatusException(Status(StatusCode::ErrorOpeningLicenseNotValid, "links object is not valid"));
                }

                auto relItem = arrayIt->FindMember("rel");
                if (relItem == arrayIt->MemberEnd())
                {
                    throw StatusException(Status(StatusCode::ErrorOpeningLicenseNotValid, "links object is not valid"));
                }
                if (!relItem->value.IsString())
                {
                    throw StatusException(Status(StatusCode::ErrorOpeningLicenseNotValid, "links object is not valid"));
                }
                
                auto name = std::string(relItem->value.GetString(), relItem->value.GetStringLength());
                if (name.empty())
                {
                    throw StatusException(Status(StatusCode::ErrorOpeningLicenseNotValid, "links object is not valid"));
                }
                if (name == Hint)
                {
                    hintFound = true;
                }

                Link link = this->ParseLinkValues(*arrayIt, reader);
                m_linksMultiMap.insert(std::make_pair(name, link));
            }

            if (!hintFound)
            {
                throw StatusException(Status(StatusCode::ErrorOpeningLicenseNotValid, "links object is not valid"));
            }

            // does nothing (links objects have no children)
            // see LcpService.spp rootNode->AddChildNode(std::move(linksNode))
            BaseLcpNode::ParseNode(linksArray, reader);
        }
        else
        {
            throw StatusException(Status(StatusCode::ErrorOpeningLicenseNotValid, "links object is not valid"));
        }
    }

    bool LinksLcpNode::GetLinks(const std::string & name, std::vector<Link> & links) const
    {
        std::pair<LinksMapConstIt, LinksMapConstIt> range = m_linksMultiMap.equal_range(name);
        std::transform(
            range.first, range.second,
            std::back_inserter(links),
            [&links](const LinksMap::value_type & val) { return val.second; }
        );
        return !links.empty();
    }

    bool LinksLcpNode::GetLink(const std::string & name, Link & link) const
    {
        auto it = m_linksMultiMap.find(name);
        if (it != m_linksMultiMap.end())
        {
            link = it->second;
            return true;
        }
        return false;
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

    IKeyValueIterator<std::string, Link> * LinksLcpNode::Enumerate() const
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