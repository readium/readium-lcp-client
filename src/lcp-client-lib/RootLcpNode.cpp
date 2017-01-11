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


#include <sstream>
#include "rapidjson/document.h"
#include "RootLcpNode.h"
#include "JsonValueReader.h"
#include "LcpUtils.h"
#include "JsonCanonicalizer.h"
#include "ICryptoProvider.h"
#include "public/ILcpService.h"

namespace lcp
{
    RootLcpNode::RootLcpNode(
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
        )
#if ENABLE_GENERIC_JSON_NODE
        : m_crypto(crypto)
        , m_links(links)
        , m_user(user)
        , m_rights(rights)
#else
        : m_crypto(std::move(std::unique_ptr<CryptoLcpNode>(crypto)))
        , m_links(std::move(std::unique_ptr<LinksLcpNode>(links)))
        , m_user(std::move(std::unique_ptr<UserLcpNode>(user)))
        , m_rights(std::move(std::unique_ptr<RightsLcpNode>(rights)))
#endif //ENABLE_GENERIC_JSON_NODE
        , m_decrypted(false)
        , m_statusDocumentProcessingFlag(false)
{
        m_rootInfo.content = licenseJson;
        m_rootInfo.canonicalContent = canonicalJson;
    }

    std::string RootLcpNode::Id() const
    {
        return m_rootInfo.id;
    }

    std::string RootLcpNode::CanonicalContent() const
    {
        return m_rootInfo.canonicalContent;
    }

    std::string RootLcpNode::OriginalContent() const
    {
        return m_rootInfo.content;
    }

    std::string RootLcpNode::Issued() const
    {
        return m_rootInfo.issued;
    }

    std::string RootLcpNode::Updated() const
    {
        return m_rootInfo.updated;
    }

    std::string RootLcpNode::Provider() const
    {
        return m_rootInfo.provider;
    }

    ICrypto * RootLcpNode::Crypto() const
    {
#if ENABLE_GENERIC_JSON_NODE
        return m_crypto;
#else
        return m_crypto.get();
#endif //ENABLE_GENERIC_JSON_NODE
    }

    ILinks * RootLcpNode::Links() const
    {
#if ENABLE_GENERIC_JSON_NODE
        return m_links;
#else
        return m_links.get();
#endif //ENABLE_GENERIC_JSON_NODE
    }

    IUser * RootLcpNode::User() const
    {
#if ENABLE_GENERIC_JSON_NODE
        return m_user;
#else
        return m_user.get();
#endif //ENABLE_GENERIC_JSON_NODE
    }

    IRights * RootLcpNode::Rights() const
    {
#if ENABLE_GENERIC_JSON_NODE
        return m_rights;
#else
        return m_rights.get();
#endif //ENABLE_GENERIC_JSON_NODE
    }

    // false by default (see constructor init)
    bool RootLcpNode::getStatusDocumentProcessingFlag() const
    {
        return m_statusDocumentProcessingFlag;
    }
    void RootLcpNode::setStatusDocumentProcessingFlag(bool flag)
    {
        m_statusDocumentProcessingFlag = flag;
    }

    bool RootLcpNode::Decrypted() const
    {
        return m_decrypted;
    }

    KeyType RootLcpNode::UserKey() const
    {
        if (m_keyProvider != nullptr)
        {
            return m_keyProvider->UserKey();
        }
        return std::move(KeyType());
    }

    KeyType RootLcpNode::ContentKey() const
    {
        if (m_keyProvider != nullptr)
        {
            return m_keyProvider->ContentKey();
        }
        return std::move(KeyType());
    }

    void RootLcpNode::SetKeyProvider(std::unique_ptr<IKeyProvider> keyProvider)
    {
        m_keyProvider = std::move(keyProvider);
    }

    Status RootLcpNode::VerifyNode(ILicense * license, IClientProvider * clientProvider, ICryptoProvider * cryptoProvider)
    {
        Status res = cryptoProvider->VerifyLicense(clientProvider->RootCertificate(), license);
        if (!Status::IsSuccess(res))
        {
            return res;
        }
#if ENABLE_GENERIC_JSON_NODE
        return BaseLcpNode::VerifyNode(license, clientProvider, cryptoProvider);
#else
        res = m_crypto->VerifyNode(license, clientProvider, cryptoProvider);
        if (!Status::IsSuccess(res))
            return res;

        res = m_links->VerifyNode(license, clientProvider, cryptoProvider);
        if (!Status::IsSuccess(res))
            return res;

        res = m_user->VerifyNode(license, clientProvider, cryptoProvider);
        if (!Status::IsSuccess(res))
            return res;

        res = m_rights->VerifyNode(license, clientProvider, cryptoProvider);
        if (!Status::IsSuccess(res))
            return res;

        return Status(StatusCode::ErrorCommonSuccess);
#endif //ENABLE_GENERIC_JSON_NODE
    }

    Status RootLcpNode::DecryptNode(ILicense * license, IKeyProvider * keyProvider, ICryptoProvider * cryptoProvider)
    {
#if ENABLE_GENERIC_JSON_NODE
        Status res = BaseLcpNode::DecryptNode(license, keyProvider, cryptoProvider);
        if (Status::IsSuccess(res))
        {
            m_decrypted = true;
        }
        return res;
#else
        Status res = m_crypto->DecryptNode(license, keyProvider, cryptoProvider);
        if (!Status::IsSuccess(res))
            return res;

        res = m_links->DecryptNode(license, keyProvider, cryptoProvider);
        if (!Status::IsSuccess(res))
            return res;

        res = m_user->DecryptNode(license, keyProvider, cryptoProvider);
        if (!Status::IsSuccess(res))
            return res;

        res = m_rights->DecryptNode(license, keyProvider, cryptoProvider);
        if (!Status::IsSuccess(res))
            return res;

        m_decrypted = true;
        return Status(StatusCode::ErrorCommonSuccess);
#endif //ENABLE_GENERIC_JSON_NODE
    }

    void RootLcpNode::ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader)
    {
        rapidjson::Document rootObject;
        if (rootObject.Parse<rapidjson::kParseValidateEncodingFlag>(m_rootInfo.content.data()).HasParseError())
        {
            throw StatusException(JsonValueReader::CreateRapidJsonError(
                rootObject.GetParseError(), rootObject.GetErrorOffset())
                );
        }

        if (!rootObject.IsObject())
        {
            throw StatusException(JsonValueReader::CreateRapidJsonError(
                rapidjson::kParseErrorValueInvalid)
                );
        }

        m_rootInfo.id = reader->ReadStringCheck("id", rootObject);
        m_rootInfo.issued = reader->ReadStringCheck("issued", rootObject);
        m_rootInfo.provider = reader->ReadStringCheck("provider", rootObject);
        m_rootInfo.updated = reader->ReadString("updated", rootObject);

#if ENABLE_GENERIC_JSON_NODE
        BaseLcpNode::ParseNode(rootObject, reader);
#else
        m_crypto->ParseNode(rootObject, reader);
        m_links->ParseNode(rootObject, reader);
        m_user->ParseNode(rootObject, reader);
        m_rights->ParseNode(rootObject, reader);
#endif //ENABLE_GENERIC_JSON_NODE
    }
}