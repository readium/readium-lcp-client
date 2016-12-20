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


#include <iostream>
#include <algorithm>
#include "rapidjson/document.h"
#include "CanonicalWriter.h"
#include "JsonValueReader.h"
#include "JsonCanonicalizer.h"
#include "LcpUtils.h"

namespace lcp
{
    JsonCanonicalizer::JsonCanonicalizer(const std::string & rawLicense, JsonValueReader * reader)
        : m_jsonReader(reader)
    {
        if (m_rootObject.Parse<rapidjson::kParseValidateEncodingFlag>(rawLicense.data()).HasParseError())
        {
            throw StatusException(JsonValueReader::CreateRapidJsonError(
                m_rootObject.GetParseError(), m_rootObject.GetErrorOffset())
                );
        }
        this->Construct();
    }

    JsonCanonicalizer::JsonCanonicalizer(rapidjson::Document rootObject, JsonValueReader * reader)
        : m_jsonReader(reader)
    {
        m_rootObject = std::move(rootObject);
        this->Construct();
    }

    void JsonCanonicalizer::Construct()
    {
        m_id = m_jsonReader->ReadStringCheck("id", m_rootObject);
        auto it = m_rootObject.FindMember("signature");
        if (it != m_rootObject.MemberEnd())
        {
            m_rootObject.RemoveMember(it);
        }
        else
        {
            throw StatusException(Status(StatusCode::ErrorOpeningLicenseNotValid, "ErrorOpeningLicenseNotValid: signature is not found"));
        }
    }

    std::string JsonCanonicalizer::CanonicalLicense()
    {
        this->SortJsonTree(m_rootObject);

        rapidjson::StringBuffer buffer;
        CanonicalWriter<rapidjson::StringBuffer> writer(buffer);
        m_rootObject.Accept(writer);

        return std::string(buffer.GetString(), buffer.GetSize());
    }

    void JsonCanonicalizer::SortJsonTree(rapidjson::Value & parentObject)
    {
        if (parentObject.IsObject())
        {
            for (auto it = parentObject.MemberBegin(); it != parentObject.MemberEnd(); ++it)
            {
                SortJsonTree(it->value);
            }

            typedef const rapidjson::GenericMember<rapidjson::UTF8<char>,
                rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> > JsonObjectIt;

            std::sort(parentObject.MemberBegin(), parentObject.MemberEnd(),
                [](JsonObjectIt & left, JsonObjectIt & right)
            {
                return LexicographicalCompareUtf8(
                    std::string(left.name.GetString(), left.name.GetStringLength()),
                    std::string(right.name.GetString(), right.name.GetStringLength())
                    );
            });
        }
        else if (parentObject.IsArray())
        {
            for (auto it = parentObject.Begin(); it != parentObject.End(); ++it)
            {
                SortJsonTree(*it);
            }
        }
    }
}