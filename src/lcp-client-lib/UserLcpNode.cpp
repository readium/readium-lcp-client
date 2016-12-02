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


#include "UserLcpNode.h"
#include "ContainerIterator.h"
#include "JsonValueReader.h"
#include "ICryptoProvider.h"

namespace lcp
{
    std::string UserLcpNode::Id() const
    {
        return m_userInfo.id;
    }

    std::string UserLcpNode::Email() const
    {
        return m_userInfo.email;
    }

    std::string UserLcpNode::Name() const
    {
        return m_userInfo.name;
    }

    bool UserLcpNode::GetUserValue(const std::string & name, std::string & value) const
    {
        auto it = m_userInfo.valuesMap.find(name);
        if (it != m_userInfo.valuesMap.end())
        {
            value = it->second;
            return true;
        }
        return false;
    }

    bool UserLcpNode::HasUserValue(const std::string & name) const
    {
        return (m_userInfo.valuesMap.find(name) != m_userInfo.valuesMap.end());
    }

    KvStringsIterator * UserLcpNode::Enumerate() const
    {
        return new MapIterator<std::string>(m_userInfo.valuesMap);
    }

#if ENABLE_GENERIC_JSON_NODE
    // noop
#else
    Status UserLcpNode::VerifyNode(ILicense * license, IClientProvider * clientProvider, ICryptoProvider * cryptoProvider)
    {
        return Status(StatusCode::ErrorCommonSuccess);
    }
#endif //ENABLE_GENERIC_JSON_NODE

    Status UserLcpNode::DecryptNode(ILicense * license, IKeyProvider * keyProvider, ICryptoProvider * cryptoProvider)
    {
        for (auto it = m_userInfo.encrypted.begin(); it != m_userInfo.encrypted.end(); ++it)
        {
            auto valueIt = m_userInfo.valuesMap.find(*it);
            if (valueIt == m_userInfo.valuesMap.end())
            {
                throw std::runtime_error("encrypted user field not found");
            }

            Status res = cryptoProvider->DecryptLicenseData(valueIt->second, license, keyProvider, valueIt->second);
            if (!Status::IsSuccess(res))
                return res;

            this->FillRegisteredFields(valueIt->first, valueIt->second);
        }

#if ENABLE_GENERIC_JSON_NODE
        return BaseLcpNode::DecryptNode(license, keyProvider, cryptoProvider);
#else
        return Status(StatusCode::ErrorCommonSuccess);
#endif //ENABLE_GENERIC_JSON_NODE
    }

    void UserLcpNode::ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader)
    {
        const rapidjson::Value & userObject = reader->ReadObject("user", parentObject);
        if (userObject.IsNull())
        {
            return;
        }

        for (auto it = userObject.MemberBegin(); it != userObject.MemberEnd(); ++it)
        {
            std::string name(it->name.GetString(), it->name.GetStringLength());

            if (name == "encrypted" && it->value.IsArray())
            {
                for (auto arrayIt = it->value.Begin(); arrayIt != it->value.End(); ++arrayIt)
                {
                    if (arrayIt->IsString())
                    {
                        m_userInfo.encrypted.push_back(
                            std::string(arrayIt->GetString(), arrayIt->GetStringLength())
                            );
                    }
                }
            }
            else
            {
                std::string strValue = reader->ConvertToString(it->value);
                bool res = m_userInfo.valuesMap.insert(std::make_pair(name, strValue)).second;
                if (!res)
                {
                    throw std::runtime_error("Duplicate user value");
                }

                FillRegisteredFields(name, std::string(it->value.GetString(), it->value.GetStringLength()));
            }
        }

#if ENABLE_GENERIC_JSON_NODE
        BaseLcpNode::ParseNode(userObject, reader);
#else
        //child->ParseNode(userObject, reader);
#endif //ENABLE_GENERIC_JSON_NODE
    }

    void UserLcpNode::FillRegisteredFields(const std::string & name, const std::string & value)
    {
        if (name == "id")
        {
            m_userInfo.id = value;
        }
        else if (name == "email")
        {
            m_userInfo.email = value;
        }
        else if (name == "name")
        {
            m_userInfo.name = value;
        }
    }
}