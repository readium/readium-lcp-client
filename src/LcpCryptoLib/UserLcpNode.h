#ifndef __USER_LCP_NODE_H__
#define __USER_LCP_NODE_H__

#include "LcpUtils.h"
#include "BaseLcpNode.h"
#include "IUser.h"

namespace lcp
{
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

        // IUser
        virtual std::string Id() const;
        virtual std::string Email() const;
        virtual std::string Name() const;
        virtual bool GetUserValue(const std::string & name, std::string & value) const;

    private:
        void FillRegisteredFields(const std::string & name, const rapidjson::Value & value);

    private:
        UserInfo m_userInfo;
    };
}

#endif //__USER_LCP_NODE_H__