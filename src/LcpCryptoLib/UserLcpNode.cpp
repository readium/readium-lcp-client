//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#include "UserLcpNode.h"
#include "JsonValueReader.h"
#include "ICryptoProvider.h"
#include "Public/ContainerIterator.h"

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
        return BaseLcpNode::DecryptNode(license, keyProvider, cryptoProvider);
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

        BaseLcpNode::ParseNode(userObject, reader);
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