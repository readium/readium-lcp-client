#ifndef __ROOT_LCP_NODE_H__
#define __ROOT_LCP_NODE_H__

#include "BaseLcpNode.h"
#include "ILicense.h"

namespace lcp
{
    struct RootInfo
    {
        std::string id;
        std::string content;
        std::string issued;
        std::string provider;
        std::string updated;
    };

    class RootLcpNode : public BaseLcpNode, public ILicense
    {
    public:
        RootLcpNode(
            const std::string & licenseJson,
            ICrypto * crypto,
            ILinks * links,
            IUser * user,
            IRights * rights
            );

    public:
        // ILcpNode
        virtual void ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader);

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
        std::string m_licenseJson;
    };
}

#endif //__ROOT_LCP_NODE_H__
