// Copyright (c) 2016 Mantano
// Licensed to the Readium Foundation under one or more contributor license agreements.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation and/or
//    other materials provided with the distribution.
// 3. Neither the name of the organization nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.



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
        const rapidjson::Value & linksObject = reader->ReadObjectCheck("links", parentObject);
        
        if (!linksObject.HasMember(Hint))
        {
            throw StatusException(Status(StatusCode::ErrorOpeningLicenseNotValid, "links object is not valid"));
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
                throw StatusException(Status(StatusCode::ErrorOpeningLicenseNotValid, "links object is not valid"));
            }
        }

        BaseLcpNode::ParseNode(linksObject, reader);
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