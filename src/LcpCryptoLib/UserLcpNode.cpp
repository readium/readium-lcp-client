#include "UserLcpNode.h"
#include "JsonValueReader.h"

namespace lcp
{
    Status UserLcpNode::ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader)
    {
        const rapidjson::Value & userObject = reader->ReadAsObject("user", parentObject);
        if (userObject.IsObject())
        {
            for (auto it = userObject.MemberBegin(); it != userObject.MemberEnd(); ++it)
            {
                std::string name = it->name.GetString();
                if (name == "id")
                    m_userInfo.id = it->value.GetString();
                else if (name == "email")
                    m_userInfo.email = it->value.GetString();
                else if (name == "name")
                    m_userInfo.name = it->value.GetString();
                else if (name == "encrypted" && it->value.IsArray())
                {
                    for (auto arrayIt = it->value.Begin(); arrayIt != it->value.End(); ++arrayIt)
                    {
                        if (arrayIt->IsString())
                        {
                            m_userInfo.Encrypted.push_back(arrayIt->GetString());
                        }
                    };
                }
                else
                {
                    m_userInfo.Extended.insert(std::make_pair(name, it->value.GetString()));
                }
            }
        }

        return Status(StCodeCover::ErrorSuccess);
    }
}