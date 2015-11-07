#include "rapidjson/document.h"
#include "JsonValueReader.h"
#include "LcpUtils.h"

namespace lcp
{
    std::string JsonValueReader::ReadAsString(const std::string & name, const rapidjson::Value & jsonValue)
    {
        auto it = jsonValue.FindMember(name.c_str());
        if (it != jsonValue.MemberEnd() && it->value.IsString())
        {
            return it->value.GetString();
        }
        return std::string();
    }

    std::string JsonValueReader::ReadAsStringCheck(const std::string & name, const rapidjson::Value & jsonValue)
    {
        auto it = jsonValue.FindMember(name.c_str());
        if (it == jsonValue.MemberEnd() || !it->value.IsString())
        {
            throw StatusException(Status(StCodeCover::ErrorLicenseNotValid, name + " object is not valid"));
        }

        std::string result(it->value.GetString());
        if (result.empty())
        {
            throw StatusException(Status(StCodeCover::ErrorLicenseNotValid, name + " object is not valid"));
        }
        return result;
    }

    const rapidjson::Value & JsonValueReader::ReadAsObject(const std::string & name, const rapidjson::Value & jsonValue)
    {
        auto it = jsonValue.FindMember(name.c_str());
        if (it != jsonValue.MemberEnd() && it->value.IsObject())
        {
            return it->value;
        }
        return rapidjson::Value();
    }

    const rapidjson::Value & JsonValueReader::ReadAsObjectCheck(const std::string & name, const rapidjson::Value & jsonValue)
    {
        auto it = jsonValue.FindMember(name.c_str());
        if (it == jsonValue.MemberEnd() || !it->value.IsObject())
        {
            throw StatusException(Status(StCodeCover::ErrorLicenseNotValid, name + " object is not valid"));
        }
        return it->value;
    }

    int JsonValueReader::ReadAsInt(const std::string & name, const rapidjson::Value & jsonValue)
    {
        auto it = jsonValue.FindMember(name.c_str());
        if (it != jsonValue.MemberEnd() && it->value.IsInt())
        {
            return it->value.GetInt();
        }
        return -1;
    }

    const rapidjson::Value & JsonValueReader::ReadAsArray(const std::string & name, const rapidjson::Value & jsonValue)
    {
        auto it = jsonValue.FindMember(name.c_str());
        if (it != jsonValue.MemberEnd() && it->value.IsArray())
        {
            return it->value;
        }
        return rapidjson::Value();
    }

    /*static*/ Status JsonValueReader::CreateRapidJsonError(rapidjson::ParseErrorCode errorCode, size_t offset /* = INVALID_OFFSET */)
    {
        std::stringstream strm;
        strm << "RapidJson error code: " << static_cast<int>(errorCode);
        if (offset != INVALID_OFFSET)
        {
            strm << ". Error offset: " << offset;
        }

        return Status(
            StCodeCover::ErrorLicenseNotValid,
            strm.str().c_str()
            );
    }
}