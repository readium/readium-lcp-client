#include "RightsLcpNode.h"
#include "JsonValueReader.h"
#include "LcpUtils.h"
#include "DateTime.h"

namespace lcp
{
    /*static*/ int RightsInfo::UNLIMITED = -1;

    bool RightsLcpNode::GetRightValue(const std::string & name, std::string & value) const
    {
        auto it = m_rights.valuesMap.find(name);
        if (it != m_rights.valuesMap.end())
        {
            value = it->second;
            return true;
        }
        return false;
    }

    Status RightsLcpNode::VerifyNode(ILicense * license, IClientProvider * clientProvider, ICryptoProvider * cryptoProvider)
    {
        /*DateTime started(m_rights.start);
        DateTime expired(m_rights.end);
        DateTime now = DateTime::Now();

        if (now < started)
        {
            return Status(StCodeCover::ErrorOpeningLicenseNotStarted);
        }
        else if (now > expired)
        {
            return Status(StCodeCover::ErrorOpeningLicenseExpired);
        }*/
        return BaseLcpNode::VerifyNode(license, clientProvider, cryptoProvider);
    }

    void RightsLcpNode::ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader)
    {
        const rapidjson::Value & rightsObject = reader->ReadObject("rights", parentObject);
        if (!rightsObject.IsNull())
        {
            for (auto it = rightsObject.MemberBegin(); it != rightsObject.MemberEnd(); ++it)
            {
                std::string name = it->name.GetString();

                std::string strValue = reader->ConvertToString(it->value);
                bool res = m_rights.valuesMap.insert(std::make_pair(name, strValue)).second;
                if (!res)
                {
                    throw std::runtime_error("Duplicate rights value");
                }

                this->FillRegisteredFields(name, it->value);
            }
        }

        BaseLcpNode::ParseNode(rightsObject, reader);
    }

    void RightsLcpNode::FillRegisteredFields(const std::string & name, const rapidjson::Value & value)
    {
        if (name == PrintRight)
        {
            m_rights.print = value.GetInt();
        }
        else if (name == CopyRight)
        {
            m_rights.copy = value.GetInt();
        }
        else if (name == TtsRight)
        {
            m_rights.tts = value.GetBool();
        }
        else if (name == StartRight)
        {
            m_rights.start.assign(value.GetString(), value.GetStringLength());
        }
        else if (name == EndRight)
        {
            m_rights.end.assign(value.GetString(), value.GetStringLength());
        }
    }
}