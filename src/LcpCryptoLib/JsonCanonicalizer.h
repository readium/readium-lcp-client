//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#ifndef __JSON_CANONICALIZER_H__
#define __JSON_CANONICALIZER_H__

#include <string>
#include "rapidjson/document.h"

namespace lcp
{
    class JsonValueReader;

    class JsonCanonicalizer
    {
    public:
        JsonCanonicalizer(const std::string & rawLicense, JsonValueReader * reader);
        JsonCanonicalizer(rapidjson::Document rootObject, JsonValueReader * reader);

        const std::string & Id() const
        {
            return m_id;
        }
        std::string CanonicalLicense();

    private:
        void Construct();
        void SortJsonTree(rapidjson::Value & parentObject);

    private:
        std::string m_id;
        rapidjson::Document m_rootObject;
        JsonValueReader * m_jsonReader;
    };
}

#endif //__JSON_CANONICALIZER_H__
