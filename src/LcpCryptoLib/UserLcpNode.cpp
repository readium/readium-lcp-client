#include "UserLcpNode.h"
#include "JsonValueReader.h"

#pragma optimize( "", off )

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

                FillRegisteredFields(name, it->value);
            }
        }

        BaseLcpNode::ParseNode(userObject, reader);
    }

    void UserLcpNode::FillRegisteredFields(const std::string & name, const rapidjson::Value & value)
    {
        if (name == "id")
        {
            m_userInfo.id.assign(value.GetString(), value.GetStringLength());
        }
        else if (name == "email")
        {
            m_userInfo.email.assign(value.GetString(), value.GetStringLength());
        }
        else if (name == "name")
        {
            m_userInfo.name.assign(value.GetString(), value.GetStringLength());
        }
    }
}