#include "RightsLcpNode.h"
#include "JsonValueReader.h"
#include "LcpUtils.h"
#include "DateTime.h"
#include "Public/ContainerIterator.h"

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

    KvStringsIterator * RightsLcpNode::Enumerate() const
    {
        return new MapIterator<std::string>(m_rights.valuesMap);
    }

    bool RightsLcpNode::HasRightValue(const std::string & name) const
    {
        return (m_rights.valuesMap.find(name) != m_rights.valuesMap.end());
    }

    bool RightsLcpNode::HasRight(const std::string & name) const
    {
        if (name == PrintRight)
        {
            return m_rights.print == RightsInfo::UNLIMITED || m_rights.print > 0;
        }
        else if (name == CopyRight)
        {
            return m_rights.copy == RightsInfo::UNLIMITED || m_rights.copy > 0;
        }
        else if (name == TtsRight)
        {
            return m_rights.tts;
        }
        else if (name == StartRight)
        {
            return this->DoesLicenseStart();
        }
        else if (name == EndRight)
        {
            return !this->DoesLicenseExpired();
        }
        return true;
    }

    bool RightsLcpNode::Consume(const std::string & name)
    {
        return this->Consume(name, 1);
    }

    bool RightsLcpNode::Consume(const std::string & name, int amount)
    {
        bool result = false;
        if (name == PrintRight)
        {
            if (m_rights.print >= amount)
            {
                m_rights.print -= amount;
                this->SetRightValueInMap(name, std::to_string(m_rights.print));
                result = true;
            }
            else if (m_rights.print == RightsInfo::UNLIMITED)
            {
                result = true;
            }
        }
        else if (name == CopyRight)
        {
            if (m_rights.copy >= amount)
            {
                m_rights.copy -= amount;
                this->SetRightValueInMap(name, std::to_string(m_rights.copy));
                result = true;
            }
            else if (m_rights.copy == RightsInfo::UNLIMITED)
            {
                result = true;
            }
        }
        return result;
    }

    void RightsLcpNode::SetRightValue(const std::string & name, const std::string & value)
    {
        if (name == PrintRight)
        {
            m_rights.print = std::stoi(value);
        }
        else if (name == CopyRight)
        {
            m_rights.copy = std::stoi(value);
        }
        else if (name == TtsRight)
        {
            m_rights.tts = StringToBool(value);
        }

        this->SetRightValueInMap(name, value);
    }

    Status RightsLcpNode::VerifyNode(ILicense * license, IClientProvider * clientProvider, ICryptoProvider * cryptoProvider)
    {
        if (!this->DoesLicenseStart())
        {
            return Status(StCodeCover::ErrorOpeningLicenseNotStarted);
        }
        if (this->DoesLicenseExpired())
        {
            return Status(StCodeCover::ErrorOpeningLicenseExpired);
        }

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

    void RightsLcpNode::SetRightValueInMap(const std::string & name, const std::string & value)
    {
        auto it = m_rights.valuesMap.find(name);
        if (it != m_rights.valuesMap.end())
        {
            it->second = value;
        }
        else
        {
            m_rights.valuesMap.insert(std::make_pair(name, value));
        }
    }

    bool RightsLcpNode::DoesLicenseStart() const
    {
        DateTime now = DateTime::Now();
        if (!m_rights.start.empty())
        {
            DateTime started(m_rights.start);
            if (now < started)
            {
                return false;
            }
        }
        return true;
    }

    bool RightsLcpNode::DoesLicenseExpired() const
    {
        DateTime now = DateTime::Now();
        if (!m_rights.end.empty())
        {
            DateTime expired(m_rights.end);
            if (now > expired)
            {
                return true;
            }
        }
        return false;
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