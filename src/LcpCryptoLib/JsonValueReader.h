#ifndef __JSON_VALUE_READER_H__
#define __JSON_VALUE_READER_H__

#include <sstream>
#include "rapidjson/document.h"
#include "Public/LcpStatus.h"

// namespace rapidjson
// {
//     class Value;
// }

namespace lcp
{
    class JsonValueReader
    {
    public:
        std::string ReadString(const std::string & name, const rapidjson::Value & jsonValue);
        std::string ReadStringCheck(const std::string & name, const rapidjson::Value & jsonValue);
        std::string ConvertToString(const rapidjson::Value & value);

        const rapidjson::Value & ReadObject(const std::string & name, const rapidjson::Value & jsonValue);
        const rapidjson::Value & ReadObjectCheck(const std::string & name, const rapidjson::Value & jsonValue);

        static Status CreateRapidJsonError(rapidjson::ParseErrorCode errorCode, size_t offset = INVALID_OFFSET);

    private:
        static const size_t INVALID_OFFSET = -1;
    };
}

#endif //__JSON_VALUE_READER_H__
