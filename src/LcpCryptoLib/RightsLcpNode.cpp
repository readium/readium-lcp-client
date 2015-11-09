#include "RightsLcpNode.h"
#include "JsonValueReader.h"

namespace lcp
{


    Status RightsLcpNode::ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader)
    {
        const rapidjson::Value & rightsObject = reader->ReadAsObject("rights", parentObject);
        if (rightsObject.IsObject())
        {
            for (auto it = rightsObject.MemberBegin(); it != rightsObject.MemberEnd(); ++it)
            {
                std::string name = it->name.GetString();
                if (name == PRINT)
                    m_rights.print = it->value.GetInt();
                else if (name == COPY)
                    m_rights.copy = it->value.GetInt();
                else if (name == TTS)
                    m_rights.tts = it->value.GetBool();
                else if (name == START)
                    m_rights.start = it->value.GetString();
                else if (name == END)
                    m_rights.end = it->value.GetString();
                else
                {
                    m_rights.extendedRights.insert(std::make_pair(name, it->value.GetString()));
                }
            }
        }


        return Status(StCodeCover::ErrorSuccess);
    }
}