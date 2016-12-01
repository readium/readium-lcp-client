// Copyright (c) 2016 Mantano
// Licensed to the Readium Foundation under one or more contributor license agreements.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation and/or
//    other materials provided with the distribution.
// 3. Neither the name of the organization nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


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
