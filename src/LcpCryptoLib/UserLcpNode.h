#ifndef __USER_LCP_NODE_H__
#define __USER_LCP_NODE_H__

#include <map>
#include "BaseLcpNode.h"
#include "IUser.h"

namespace lcp
{
    struct UserInfo
    {
        std::string Id;
        std::string Email;
        std::string Name;
        std::vector<std::string> Encrypted;
        std::map<std::string, std::string> Extended;
    };

    class UserLcpNode : public BaseLcpNode, public IUser
    {
    public:
        // ILcpNode
        virtual Status ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader);

    private:
        UserInfo m_userInfo;
    };
}

#endif //__USER_LCP_NODE_H__