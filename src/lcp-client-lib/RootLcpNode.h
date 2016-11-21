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

#if ENABLE_GENERIC_JSON_NODE
// noop
#else

#include "CryptoLcpNode.h"
#include "LinksLcpNode.h"
#include "UserLcpNode.h"
#include "RightsLcpNode.h"

#endif //ENABLE_GENERIC_JSON_NODE

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

#if ENABLE_GENERIC_JSON_NODE
            ICrypto * crypto,
            ILinks * links,
            IUser * user,
            IRights * rights
#else
            CryptoLcpNode * crypto,
            LinksLcpNode * links,
            UserLcpNode * user,
            RightsLcpNode * rights
#endif //ENABLE_GENERIC_JSON_NODE
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

#if !DISABLE_LSD
        virtual bool getStatusDocumentProcessingFlag() const;
        virtual void setStatusDocumentProcessingFlag(bool flag);
#endif //!DISABLE_LSD
    public:
        virtual KeyType UserKey() const;
        virtual KeyType ContentKey() const;

    private:
        RootInfo m_rootInfo;

#if ENABLE_GENERIC_JSON_NODE
        ICrypto * m_crypto;
        ILinks * m_links;
        IUser * m_user;
        IRights * m_rights;
#else
        std::unique_ptr<CryptoLcpNode> m_crypto;
        std::unique_ptr<LinksLcpNode> m_links;
        std::unique_ptr<UserLcpNode> m_user;
        std::unique_ptr<RightsLcpNode> m_rights;
#endif //ENABLE_GENERIC_JSON_NODE

        bool m_decrypted;

#if !DISABLE_LSD
        bool m_statusDocumentProcessingFlag;
#endif //!DISABLE_LSD

        std::unique_ptr<IKeyProvider> m_keyProvider;
    };
}

#endif //__ROOT_LCP_NODE_H__
