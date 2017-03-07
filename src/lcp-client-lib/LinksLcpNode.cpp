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
        auto linksMember = parentObject.FindMember("links");
        if (linksMember == parentObject.MemberEnd())
        {
            throw StatusException(Status(StatusCode::ErrorOpeningLicenseNotValid, "ErrorOpeningLicenseNotValid: links object is not valid"));
        }

        // linksMember->name == "links"

// Alternative parsing method:
//        const rapidjson::Value & linksObject = reader->ReadObject("links", parentObject);
//        if (linksObject.IsNull()) {
//            const rapidjson::Value &linksArray = reader->ReadArray("links", parentObject);
//
//            rapidjson::Type type = linksArray.GetType();
//            if (type != rapidjson::kArrayType) { // linksArray.isArray()
//                throw StatusException(Status(StatusCode::ErrorOpeningLicenseNotValid, "ErrorOpeningLicenseNotValid: links array is not valid type?!"));
//            }
//            //...
//        } else {
//
//            rapidjson::Type type = linksObject.GetType();
//            if (type != rapidjson::kObjectType) { // linksObject.isObject()
//                throw StatusException(Status(StatusCode::ErrorOpeningLicenseNotValid, "ErrorOpeningLicenseNotValid: links object is not valid type?!"));
//            }
//            //...
//        }

        // legacy JSON format, links is object map (now array of objects with "rel" field instead of object key)
        if (linksMember->value.IsObject())
        {
            const rapidjson::Value & linksObject = linksMember->value; //reader->ReadObjectCheck("links", parentObject);
            
            if (!linksObject.HasMember(Hint))
            {
                throw StatusException(Status(StatusCode::ErrorOpeningLicenseNotValid, "ErrorOpeningLicenseNotValid: links object does not contain HINT"));
            }

            for (auto linksMember = linksObject.MemberBegin(); linksMember != linksObject.MemberEnd(); ++linksMember)
            {
                auto name = std::string(linksMember->name.GetString(), linksMember->name.GetStringLength());

                rapidjson::Type type = linksMember->value.GetType();

                bool checkHref = name != Hint;

                if (type == rapidjson::kObjectType)
                {
                    const rapidjson::Value & linkObject = linksMember->value;

                    Link link = this->ParseLinkValues(linkObject, reader, checkHref);
                    m_linksMultiMap.insert(std::make_pair(name, link));
                }
                else if (type == rapidjson::kArrayType)
                {
                    for (auto linkObject = linksMember->value.Begin(); linkObject != linksMember->value.End(); ++linkObject)
                    {
                        Link link = this->ParseLinkValues(*linkObject, reader, checkHref);
                        m_linksMultiMap.insert(std::make_pair(name, link));
                    }
                }
                else
                {
                    throw StatusException(Status(StatusCode::ErrorOpeningLicenseNotValid, "ErrorOpeningLicenseNotValid: links object is not valid"));
                }
            }

#if ENABLE_GENERIC_JSON_NODE
            BaseLcpNode::ParseNode(linksObject, reader);
#else
            //child->ParseNode(linksObject, reader);
#endif //ENABLE_GENERIC_JSON_NODE
        }
        else if (linksMember->value.IsArray())
        {
            const rapidjson::Value & linksArray = linksMember->value;

            bool hintFound = false;

            for (auto linkObject = linksArray.Begin(); linkObject != linksArray.End(); ++linkObject)
            {
                if (!linkObject->IsObject())
                {
                    throw StatusException(Status(StatusCode::ErrorOpeningLicenseNotValid, "ErrorOpeningLicenseNotValid: links object is not valid"));
                }

                auto relItem = linkObject->FindMember("rel");
                if (relItem == linkObject->MemberEnd())
                {
                    throw StatusException(Status(StatusCode::ErrorOpeningLicenseNotValid, "ErrorOpeningLicenseNotValid: links object is not valid"));
                }
                if (!relItem->value.IsString())
                {
                    throw StatusException(Status(StatusCode::ErrorOpeningLicenseNotValid, "ErrorOpeningLicenseNotValid: links object is not valid"));
                }
                
                auto name = std::string(relItem->value.GetString(), relItem->value.GetStringLength());
                if (name.empty())
                {
                    throw StatusException(Status(StatusCode::ErrorOpeningLicenseNotValid, "ErrorOpeningLicenseNotValid: links object is not valid"));
                }

                bool checkHref = true;
                if (name == Hint)
                {
                    hintFound = true;
                    checkHref = false;
                }
                Link link = this->ParseLinkValues(*linkObject, reader, checkHref);
                m_linksMultiMap.insert(std::make_pair(name, link));
            }

            if (!hintFound)
            {
                throw StatusException(Status(StatusCode::ErrorOpeningLicenseNotValid, "ErrorOpeningLicenseNotValid: links array does not contain HINT"));
            }

#if ENABLE_GENERIC_JSON_NODE
            BaseLcpNode::ParseNode(linksArray, reader);
#else
            //child->ParseNode(linksArray, reader);
#endif //ENABLE_GENERIC_JSON_NODE
        }
        else
        {
            throw StatusException(Status(StatusCode::ErrorOpeningLicenseNotValid, "ErrorOpeningLicenseNotValid: links object is not valid"));
        }
    }

#if ENABLE_GENERIC_JSON_NODE
    // noop
#else
    Status LinksLcpNode::DecryptNode(ILicense * license, IKeyProvider * keyProvider, ICryptoProvider * cryptoProvider)
    {
        return Status(StatusCode::ErrorCommonSuccess);
    }
#endif //ENABLE_GENERIC_JSON_NODE

#if ENABLE_GENERIC_JSON_NODE
    // noop
#else
    Status LinksLcpNode::VerifyNode(ILicense * license, IClientProvider * clientProvider, ICryptoProvider * cryptoProvider)
    {
        return Status(StatusCode::ErrorCommonSuccess);
    }
#endif //ENABLE_GENERIC_JSON_NODE


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

    Link LinksLcpNode::ParseLinkValues(const rapidjson::Value & linkObject, JsonValueReader * reader, bool checkHref)
    {
        Link link;
        if (checkHref) {
            link.href = reader->ReadStringCheck("href", linkObject);
        } else {
            link.href = reader->ReadString("href", linkObject);
        }
        link.title = reader->ReadString("title", linkObject);
        link.type = reader->ReadString("type", linkObject);
        link.templated = reader->ReadBoolean("templated", linkObject);
        link.length = reader->ReadInteger("length", linkObject);
        link.hash = reader->ReadString("hash", linkObject);
        return link;
    }
}