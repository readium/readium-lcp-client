// Copyright (c) 2016 Mantano, Readium.
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
            m_rights.print = StringToInt(value);
        }
        else if (name == CopyRight)
        {
            m_rights.copy = StringToInt(value);
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
                this->SetRightValueInMap(name, ToString(m_rights.print));
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
                this->SetRightValueInMap(name, ToString(m_rights.copy));
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
            this->SetRightValueInMap(PrintRight, ToString(m_rights.print));
        }
        if (m_rights.valuesMap.find(CopyRight) == m_rights.valuesMap.end())
        {
            this->SetRightValueInMap(CopyRight, ToString(m_rights.copy));
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