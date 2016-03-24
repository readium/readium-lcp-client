//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#ifndef __ROOT_LCP_NODE_H__
#define __ROOT_LCP_NODE_H__

#include "BaseLcpNode.h"
#include "public/ILicense.h"
#include "IKeyProvider.h"

namespace lcp
{
    struct RootInfo
    {
        std::string id;
        std::string content;
        std::string canonicalContent;
        std::string issued;
        std::string provider;
        std::string updated;
    };
    
    class RootLcpNode : public BaseLcpNode, public ILicense, public IKeyProvider
    {
    public:
        RootLcpNode(
            const std::string & licenseJson,
            const std::string & canonicalJson,
            ICrypto * crypto,
            ILinks * links,
            IUser * user,
            IRights * rights
            );

        void SetKeyProvider(std::unique_ptr<IKeyProvider> keyProvider);

    public:
        // ILcpNode
        virtual void ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader);
        virtual Status VerifyNode(ILicense * license, IClientProvider * clientProvider, ICryptoProvider * cryptoProvider);
        virtual Status DecryptNode(ILicense * license, IKeyProvider * keyProvider, ICryptoProvider * cryptoProvider);

    public:
        // ILicense
        virtual std::string Id() const;
        virtual std::string CanonicalContent() const;
        virtual std::string OriginalContent() const;
        virtual std::string Issued() const;
        virtual std::string Updated() const;
        virtual std::string Provider() const;

        virtual ICrypto * Crypto() const;
        virtual ILinks * Links() const;
        virtual IUser * User() const;
        virtual IRights * Rights() const;

        virtual bool Decrypted() const;

    public:
        virtual KeyType UserKey() const;
        virtual KeyType ContentKey() const;

    private:
        RootInfo m_rootInfo;
        ICrypto * m_crypto;
        ILinks * m_links;
        IUser * m_user;
        IRights * m_rights;
        bool m_decrypted;
        std::unique_ptr<IKeyProvider> m_keyProvider;
    };
}

#endif //__ROOT_LCP_NODE_H__
