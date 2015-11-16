#include "rapidjson/document.h"
#include "RootLcpNode.h"
#include "JsonValueReader.h"
#include "LcpUtils.h"
#include "JsonCanonicalizer.h"

namespace lcp
{
    RootLcpNode::RootLcpNode(
        const std::string & licenseJson,
        std::string canonicalJson,
        ICrypto * crypto,
        ILinks * links,
        IUser * user,
        IRights * rights
        )
        : m_licenseJson(licenseJson)
        , m_crypto(crypto)
        , m_links(links)
        , m_user(user)
        , m_rights(rights)
    {
        m_rootInfo.content = std::move(canonicalJson);
    }

    std::string RootLcpNode::Id() const
    {
        return m_rootInfo.id;
    }

    const std::string & RootLcpNode::Content() const
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

    void RootLcpNode::ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader)
    {
        rapidjson::Document rootObject;
        if (rootObject.Parse<rapidjson::kParseValidateEncodingFlag>(m_licenseJson.data()).HasParseError())
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

        BaseLcpNode::ParseNode(rootObject, reader);

        //JsonCanonicalizer canonicalizer(std::move(rootObject), reader);
        //m_rootInfo.content = canonicalizer.CanonicalLicense();
    }
}