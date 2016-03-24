//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#ifndef __USER_LCP_NODE_H__
#define __USER_LCP_NODE_H__

#include "LcpTypedefs.h"
#include "BaseLcpNode.h"
#include "public/IUser.h"

namespace lcp
{
    class ILicense;

    struct UserInfo
    {
        std::string id;
        std::string email;
        std::string name;
        std::vector<std::string> encrypted;
        StringsMap valuesMap;
    };

    class UserLcpNode : public BaseLcpNode, public IUser
    {
    public:
        // ILcpNode
        virtual void ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader);
        virtual Status DecryptNode(ILicense * license, IKeyProvider * keyProvider, ICryptoProvider * cryptoProvider);

    public:
        // IUser
        virtual std::string Id() const;
        virtual std::string Email() const;
        virtual std::string Name() const;
        virtual bool HasUserValue(const std::string & name) const;
        virtual bool GetUserValue(const std::string & name, std::string & value) const;
        virtual KvStringsIterator * Enumerate() const;

    private:
        void FillRegisteredFields(const std::string & name, const std::string & value);

    private:
        UserInfo m_userInfo;
    };
}

#endif //__USER_LCP_NODE_H__