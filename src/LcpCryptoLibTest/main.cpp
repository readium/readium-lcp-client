#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <iostream>

using namespace rapidjson;

int main()
{
    const char jsonLicense[] = "{ \"id\": \"ef15e740-697f-11e3-949a-0800200c9a66\","
        "\"issued\": \"2013-11-04T01:08:15+01:00\","
        "\"updated\": \"2014-02-21T09:44:17+01:00\","
        "\"provider\": \"http://www.imaginaryebookretailer.com\","
        "\"encryption\": {"
        "\"profile\": \"http://readium.org/lcp/profile-1.0\","
        "\"content_key\": {"
        "\"encrypted_value\": \"/k8RpXqf4E2WEunCp76E8PjhS051NXwAXeTD1ioazYxCRGvHLAck/KQ3cCh5JxDmCK0nRLyAxs1X0aA3z55boQ==\","
        "\"algorithm\": \"http://www.w3.org/2001/04/xmlenc#aes256-cbc\""
        "},"
        "\"user_key\": {"
        "\"text_hint\": \"Enter your email address\","
        "\"algorithm\": \"http://www.w3.org/2001/04/xmlenc#sha256\","
        "\"key_check\": \"jJEjUDipHK3OjGt6kFq7dcOLZuicQFUYwQ+TYkAIWKm6Xv6kpHFhF7LOkUK/Owww\""
        "},"
        "},"
        "}";

    const char json[] = " { \"hello\" : \"world\", \"t\" : true , \"f\" : false, \"n\": null, \"i\":123, \"pi\": 3.1416, \"a\":[1, 2, 3, 4] } ";
    Document testDoc;
    testDoc.Parse(json);
    Type tp0 = testDoc.GetType();

    Value::ConstMemberIterator it = testDoc.FindMember("hello");
    if (it != testDoc.MemberEnd())
    {
        ;//
    }

    Document doc;
    if (doc.Parse(jsonLicense).HasParseError())
    {
        //CreateError
    }

    Type tp = doc.GetType();
    for (Value::ConstMemberIterator it = doc.MemberBegin(); it != doc.MemberEnd(); ++it)
    {
        Type type = it->value.GetType();
        std::string name = it->name.GetString();
    }

    std::cin.get();
}