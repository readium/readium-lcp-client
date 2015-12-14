//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright � 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#include "RightsLcpNode.h"
#include "JsonValueReader.h"
#include "LcpUtils.h"
#include "ContainerIterator.h"
#include "DateTime.h"

namespace lcp
{
    void RightsLcpNode::ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader)
    {
        std::unique_lock<std::mutex> locker(m_rightsSync);

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
        this->SetDefaultRightValuesInMap();
        
        locker.unlock();

        BaseLcpNode::ParseNode(rightsObject, reader);
    }

    Status RightsLcpNode::VerifyNode(ILicense * license, IClientProvider * clientProvider, ICryptoProvider * cryptoProvider)
    {
        std::unique_lock<std::mutex> locker(m_rightsSync);

        if (!this->DoesLicenseStart())
        {
            return Status(StatusCode::ErrorOpeningLicenseNotStarted);
        }
        if (this->DoesLicenseExpired())
        {
            return Status(StatusCode::ErrorOpeningLicenseExpired);
        }

        locker.unlock();

        return BaseLcpNode::VerifyNode(license, clientProvider, cryptoProvider);
    }

    KvStringsIterator * RightsLcpNode::Enumerate() const
    {
        std::unique_lock<std::mutex> locker(m_rightsSync);
        return new MapIterator<std::string>(m_rights.valuesMap);
    }

    bool RightsLcpNode::HasRightValue(const std::string & name) const
    {
        std::unique_lock<std::mutex> locker(m_rightsSync);
        return (m_rights.valuesMap.find(name) != m_rights.valuesMap.end());
    }

    bool RightsLcpNode::GetRightValue(const std::string & name, std::string & value) const
    {
        std::unique_lock<std::mutex> locker(m_rightsSync);

        auto it = m_rights.valuesMap.find(name);
        if (it != m_rights.valuesMap.end())
        {
            value = it->second;
            return true;
        }
        return false;
    }

    void RightsLcpNode::SetRightValue(const std::string & name, const std::string & value)
    {
        std::unique_lock<std::mutex> locker(m_rightsSync);

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

    bool RightsLcpNode::UseRight(const std::string & name)
    {
        return this->UseRight(name, 1);
    }

    bool RightsLcpNode::UseRight(const std::string & name, int amount)
    {
        std::unique_lock<std::mutex> locker(m_rightsSync);

        bool result = false;
        if (name == PrintRight)
        {
            if (m_rights.print >= amount)
            {
                m_rights.print -= amount;
                this->SetRightValueInMap(name, std::to_string(m_rights.print));
                result = true;
            }
            else if (m_rights.print == IRightsService::UNLIMITED)
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
            else if (m_rights.copy == IRightsService::UNLIMITED)
            {
                result = true;
            }
        }
        return result;
    }

    bool RightsLcpNode::CanUseRight(const std::string & name) const
    {
        std::unique_lock<std::mutex> locker(m_rightsSync);

        if (name == PrintRight)
        {
            return m_rights.print == IRightsService::UNLIMITED || m_rights.print > 0;
        }
        else if (name == CopyRight)
        {
            return m_rights.copy == IRightsService::UNLIMITED || m_rights.copy > 0;
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
        else
        {
            throw std::invalid_argument("only rights part of the specification are recognized");
        }
    }

    void RightsLcpNode::SetDefaultRightValuesInMap()
    {
        if (m_rights.valuesMap.find(PrintRight) == m_rights.valuesMap.end())
        {
            this->SetRightValueInMap(PrintRight, std::to_string(m_rights.print));
        }
        if (m_rights.valuesMap.find(CopyRight) == m_rights.valuesMap.end())
        {
            this->SetRightValueInMap(CopyRight, std::to_string(m_rights.copy));
        }
        if (m_rights.valuesMap.find(TtsRight) == m_rights.valuesMap.end())
        {
            this->SetRightValueInMap(CopyRight, BoolToString(m_rights.tts));
        }
    }

    void RightsLcpNode::SetRightValueInMap(const std::string & name, const std::string & value)
    {
        auto it = m_rights.valuesMap.find(name);
        if (it != m_rights.valuesMap.end() && it->second != value)
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