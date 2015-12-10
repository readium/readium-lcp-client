//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//  Licensed under Gnu Affero General Public License Version 3 (provided, notwithstanding this notice,
//  Readium Foundation reserves the right to license this material under a different separate license,
//  and if you have done so, the terms of that separate license control and the following references
//  to GPL do not apply).
//
//  This program is free software: you can redistribute it and/or modify it under the terms of the GNU
//  Affero General Public License as published by the Free Software Foundation, either version 3 of
//  the License, or (at your option) any later version. You should have received a copy of the GNU
//  Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

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
            throw StatusException(Status(StatusCode::ErrorOpeningLicenseNotValid, "signature is not found"));
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