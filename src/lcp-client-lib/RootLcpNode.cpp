//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

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
        : m_crypto(crypto)
        , m_links(links)
        , m_user(user)
        , m_rights(rights)
        , m_decrypted(false)
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
        return m_crypto;
    }

    ILinks * RootLcpNode::Links() const
    {
        return m_links;
    }

    IUser * RootLcpNode::User() const
    {
        return m_user;
    }

    IRights * RootLcpNode::Rights() const
    {
        return m_rights;
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