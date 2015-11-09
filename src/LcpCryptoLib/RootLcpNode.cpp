#include "rapidjson/document.h"
#include "RootLcpNode.h"
#include "JsonValueReader.h"

namespace lcp
{
    RootLcpNode::RootLcpNode(ICrypto * crypto, ILinks * links, IUser * user, IRights * rights)
        : m_crypto(crypto)
        , m_links(links)
        , m_user(user)
        , m_rights(rights)
    {
    }

    std::string RootLcpNode::Id() const
    {
        return m_rootInfo.id;
    }

    std::string RootLcpNode::Content() const
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

    Status RootLcpNode::ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader)
    {
        if (!parentObject.IsObject())
        {
            return JsonValueReader::CreateRapidJsonError(rapidjson::kParseErrorValueInvalid);
        }

        m_rootInfo.id = reader->ReadAsStringCheck("id", parentObject);
        m_rootInfo.issued = reader->ReadAsStringCheck("issued", parentObject);
        m_rootInfo.provider = reader->ReadAsStringCheck("provider", parentObject);
        m_rootInfo.updated = reader->ReadAsString("updated", parentObject);

        for (auto it = m_childs.begin(); it != m_childs.end(); ++it)
        {
            Status res = (*it)->ParseNode(parentObject, reader);
            if (!Status::IsSuccess(res))
            {
                return res;
            }
        }
        return Status(StCodeCover::ErrorSuccess);
    }
}