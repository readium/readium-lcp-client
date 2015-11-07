#include "RightsLcpNode.h"
#include "JsonValueReader.h"

namespace lcp
{
    /*static*/ const char * Rights::PRINT = "print";
    /*static*/ const char * Rights::COPY = "copy";
    /*static*/ const char * Rights::TTS = "tts";
    /*static*/ const char * Rights::START = "start";
    /*static*/ const char * Rights::END = "end";

    Status RightsLcpNode::ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader)
    {
        const rapidjson::Value & rightsObject = reader->ReadAsObject("rights", parentObject);
        if (rightsObject.IsObject())
        {
            for (auto it = rightsObject.MemberBegin(); it != rightsObject.MemberEnd(); ++it)
            {
                std::string name = it->name.GetString();
                if (name == Rights::PRINT)
                    m_rights.Print = it->value.GetInt();
                else if (name == Rights::COPY)
                    m_rights.Copy = it->value.GetInt();
                else if (name == Rights::TTS)
                    m_rights.Tts = it->value.GetBool();
                else if (name == Rights::START)
                    m_rights.Start = it->value.GetString();
                else if (name == Rights::END)
                    m_rights.End = it->value.GetString();
                else
                {
                    m_rights.ExtendedRights.insert(std::make_pair(name, it->value.GetString()));
                }
            }
        }


        return Status(StCodeCover::ErrorSuccess);
    }
}