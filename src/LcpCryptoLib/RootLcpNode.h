#ifndef __ROOT_LCP_NODE_H__
#define __ROOT_LCP_NODE_H__

#include "BaseLcpNode.h"
#include "ILicense.h"

namespace lcp
{
    struct RootInfo
    {
        std::string Id;
        std::string Content;
        std::string Issued;
        std::string Provider;
        std::string Updated;
    };

    class RootLcpNode : public BaseLcpNode, public ILicense
    {
    public:
        RootLcpNode(
            ICrypto * crypto,
            ILinks * links,
            IUser * user,
            IRights * rights
            );

    public:
        // ILcpNode
        virtual Status ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader);

    public:
        // ILicense
        virtual std::string Id() const;
        virtual std::string Content() const;
        virtual std::string Issued() const;
        virtual std::string Updated() const;
        virtual std::string Provider() const;

        virtual ICrypto * Crypto() const;
        virtual ILinks * Links() const;
        virtual IUser * User() const;
        virtual IRights * Rights() const;

    private:
        RootInfo m_rootInfo;
        ICrypto * m_crypto;
        ILinks * m_links;
        IUser * m_user;
        IRights * m_rights;
    };
}

#endif //__ROOT_LCP_NODE_H__
