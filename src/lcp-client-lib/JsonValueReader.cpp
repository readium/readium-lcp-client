//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#include "JsonValueReader.h"
#include "LcpUtils.h"

namespace lcp
{
    std::string JsonValueReader::ReadString(const std::string & name, const rapidjson::Value & jsonValue)
    {
        auto it = jsonValue.FindMember(name.c_str());
        if (it != jsonValue.MemberEnd() && it->value.IsString())
        {
            return std::string(it->value.GetString(), it->value.GetStringLength());
        }
        return std::string();
    }

    std::string JsonValueReader::ReadStringCheck(const std::string & name, const rapidjson::Value & jsonValue)
    {
        auto it = jsonValue.FindMember(name.c_str());
        if (it == jsonValue.MemberEnd() || !it->value.IsString())
        {
            throw StatusException(Status(StatusCode::ErrorOpeningLicenseNotValid, name + " object is not valid"));
        }

        std::string result(it->value.GetString(), it->value.GetStringLength());
        if (result.empty())
        {
            throw StatusException(Status(StatusCode::ErrorOpeningLicenseNotValid, name + " object is not valid"));
        }
        return result;
    }

    const rapidjson::Value & JsonValueReader::ReadObject(const std::string & name, const rapidjson::Value & jsonValue)
    {
        auto it = jsonValue.FindMember(name.c_str());
        if (it != jsonValue.MemberEnd() && it->value.IsObject())
        {
            return it->value;
        }
        return rapidjson::Value(rapidjson::kNullType).Move();
    }

    const rapidjson::Value & JsonValueReader::ReadObjectCheck(const std::string & name, const rapidjson::Value & jsonValue)
    {
        auto it = jsonValue.FindMember(name.c_str());
        if (it == jsonValue.MemberEnd() || !it->value.IsObject())
        {
            throw StatusException(Status(StatusCode::ErrorOpeningLicenseNotValid, name + " object is not valid"));
        }
        return it->value;
    }

    bool JsonValueReader::ReadBoolean(const std::string & name, const rapidjson::Value & jsonValue)
    {
        auto it = jsonValue.FindMember(name.c_str());
        if (it != jsonValue.MemberEnd() && it->value.IsBool())
        {
            return jsonValue.GetBool();
        }
        return false;
    }

    int JsonValueReader::ReadInteger(const std::string & name, const rapidjson::Value & jsonValue)
    {
        auto it = jsonValue.FindMember(name.c_str());
        if (it != jsonValue.MemberEnd() && it->value.IsInt())
        {
            return jsonValue.GetInt();
        }
        return 0;
    }

    std::string JsonValueReader::ConvertToString(const rapidjson::Value & value)
    {
        switch (value.GetType())
        {
        case rapidjson::kFalseType:
            return BoolToString(value.GetBool());
        case rapidjson::kTrueType:
            return BoolToString(value.GetBool());
        case rapidjson::kStringType:
            return std::string(value.GetString(), value.GetStringLength());
        case rapidjson::kNumberType:
            if (value.IsInt())
                return std::to_string(value.GetInt());
            else if (value.IsUint())
                return std::to_string(value.GetUint());
            else if (value.IsInt64())
                return std::to_string(value.GetInt64());
            else if (value.IsUint64())
                return std::to_string(value.GetUint64());
            else
                return std::to_string(value.GetDouble());
        case rapidjson::kNullType:
        case rapidjson::kObjectType:
        case rapidjson::kArrayType:
        default:
            return std::string();
        }
    }

    /*static*/ Status JsonValueReader::CreateRapidJsonError(rapidjson::ParseErrorCode errorCode, size_t offset /* = INVALID_OFFSET */)
    {
        std::stringstream strm;
        strm << "RapidJson error code: " << static_cast<int>(errorCode);
        if (offset != InvalidOffset)
        {
            strm << ". Error offset: " << offset;
        }

        return Status(
            StatusCode::ErrorOpeningLicenseNotValid,
            strm.str().c_str()
            );
    }
}