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
            throw StatusException(Status(StCodeCover::ErrorOpeningLicenseNotValid, "signature is not found"));
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