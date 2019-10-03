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
            throw StatusException(Status(StatusCode::ErrorOpeningLicenseNotValid, "ErrorOpeningLicenseNotValid: " + name + " object is not valid"));
        }

        std::string result(it->value.GetString(), it->value.GetStringLength());
        if (result.empty())
        {
            throw StatusException(Status(StatusCode::ErrorOpeningLicenseNotValid, "ErrorOpeningLicenseNotValid: " + name + " object is not valid"));
        }
        return result;
    }

    const rapidjson::Value & JsonValueReader::ReadArray(const std::string & name, const rapidjson::Value & jsonValue)
    {
        auto it = jsonValue.FindMember(name.c_str());
        if (it != jsonValue.MemberEnd() && it->value.IsArray())
        {
            return it->value;
        }

        // https://rapidjson.org/md_doc_tutorial.html#TemporaryValues
        // https://github.com/Tencent/rapidjson/issues/387#issuecomment-122067853
        // std::move() not needed (same as other returns, like it->value above)
        // rapidjson::Value(rapidjson::kNullType).Move();
        // rapidjson::Value().SetNull());
        return rapidjson::Document().SetNull(); // THIS WORKS!! (allocator is preserved)
    }

    const rapidjson::Value & JsonValueReader::ReadArrayCheck(const std::string & name, const rapidjson::Value & jsonValue)
    {
        auto it = jsonValue.FindMember(name.c_str());
        if (it == jsonValue.MemberEnd() || !it->value.IsArray())
        {
            throw StatusException(Status(StatusCode::ErrorOpeningLicenseNotValid, "ErrorOpeningLicenseNotValid: " + name + " array is not valid"));
        }
        return it->value;
    }

    const rapidjson::Value & JsonValueReader::ReadObject(const std::string & name, const rapidjson::Value & jsonValue)
    {
        auto it = jsonValue.FindMember(name.c_str());
        if (it != jsonValue.MemberEnd() && it->value.IsObject())
        {
            return it->value;
        }

        // https://rapidjson.org/md_doc_tutorial.html#TemporaryValues
        // https://github.com/Tencent/rapidjson/issues/387#issuecomment-122067853
        // std::move() not needed (same as other returns, like it->value above)
        // rapidjson::Value(rapidjson::kNullType).Move();
        // rapidjson::Value().SetNull());
        return rapidjson::Document().SetNull(); // THIS WORKS!! (allocator is preserved)
    }

    const rapidjson::Value & JsonValueReader::ReadObjectCheck(const std::string & name, const rapidjson::Value & jsonValue)
    {
        auto it = jsonValue.FindMember(name.c_str());
        if (it == jsonValue.MemberEnd() || !it->value.IsObject())
        {
            throw StatusException(Status(StatusCode::ErrorOpeningLicenseNotValid, "ErrorOpeningLicenseNotValid: " + name + " object is not valid"));
        }
        return it->value;
    }

    bool JsonValueReader::ReadBoolean(const std::string & name, const rapidjson::Value & jsonValue)
    {
        auto it = jsonValue.FindMember(name.c_str());
        if (it != jsonValue.MemberEnd() && it->value.IsBool())
        {
            return it->value.GetBool();
        }
        return false;
    }

    int JsonValueReader::ReadInteger(const std::string & name, const rapidjson::Value & jsonValue)
    {
        auto it = jsonValue.FindMember(name.c_str());
        if (it != jsonValue.MemberEnd() && it->value.IsInt())
        {
            return it->value.GetInt();
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
                return ToString(value.GetInt());
            else if (value.IsUint())
                return ToString(value.GetUint());
            else if (value.IsInt64())
                return ToString(value.GetInt64());
            else if (value.IsUint64())
                return ToString(value.GetUint64());
            else
                return ToString(value.GetDouble());
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