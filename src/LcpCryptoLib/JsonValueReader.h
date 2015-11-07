#ifndef __JSON_VALUE_READER_H__
#define __JSON_VALUE_READER_H__

#include <sstream>
#include "rapidjson/document.h"
#include "LcpStatus.h"

// namespace rapidjson
// {
//     class Value;
// }

namespace lcp
{
    class JsonValueReader
    {
    public:
        std::string ReadAsString(const std::string & name, const rapidjson::Value & jsonValue);
        std::string ReadAsStringCheck(const std::string & name, const rapidjson::Value & jsonValue);

        const rapidjson::Value & ReadAsObject(const std::string & name, const rapidjson::Value & jsonValue);
        const rapidjson::Value & ReadAsObjectCheck(const std::string & name, const rapidjson::Value & jsonValue);

        int ReadAsInt(const std::string & name, const rapidjson::Value & jsonValue);
        const rapidjson::Value & ReadAsArray(const std::string & name, const rapidjson::Value & jsonValue);

        static Status CreateRapidJsonError(rapidjson::ParseErrorCode errorCode, size_t offset = INVALID_OFFSET);

    private:
        static const size_t INVALID_OFFSET = -1;
    };
}

#endif //__JSON_VALUE_READER_H__
