#include "rapidjson/document.h"
#include "CryptoLcpNode.h"
#include "JsonValueReader.h"

namespace lcp
{
    std::string CryptoLcpNode::EncryptionProfile() const
    {
        return m_cryptoInfo.encryptionProfile;
    }

    std::string CryptoLcpNode::ContentKey() const
    {
        return m_cryptoInfo.contentKey;
    }

    std::string CryptoLcpNode::ContentKeyAlgorithm() const
    {
        return m_cryptoInfo.contentKeyAlgorithm;
    }

    std::string CryptoLcpNode::UserKeyHint() const
    {
        return m_cryptoInfo.userKeyHint;
    }

    std::string CryptoLcpNode::UserKeyAlgorithm() const
    {
        return m_cryptoInfo.userKeyAlgorithm;
    }

    std::string CryptoLcpNode::UserKeyCheck() const
    {
        return m_cryptoInfo.userKeyCheck;
    }

    std::string CryptoLcpNode::Signature() const
    {
        return m_cryptoInfo.signature;
    }

    std::string CryptoLcpNode::SignatureAlgorithm() const
    {
        return m_cryptoInfo.signatureAlgorithm;
    }

    std::string CryptoLcpNode::SignatureCertificate() const
    {
        return m_cryptoInfo.signatureCertificate;
    }

    void CryptoLcpNode::ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader)
    {
        const rapidjson::Value & encryptionObject = reader->ReadObjectCheck("encryption", parentObject);

        m_cryptoInfo.encryptionProfile = reader->ReadStringCheck("profile", encryptionObject);

        const rapidjson::Value & contentKeyObject = reader->ReadObjectCheck("content_key", encryptionObject);

        m_cryptoInfo.contentKey = reader->ReadStringCheck("encrypted_value", contentKeyObject);
        m_cryptoInfo.contentKeyAlgorithm = reader->ReadStringCheck("algorithm", contentKeyObject);

        const rapidjson::Value & userKeyObject = reader->ReadObjectCheck("user_key", encryptionObject);

        m_cryptoInfo.userKeyHint = reader->ReadStringCheck("text_hint", userKeyObject);
        m_cryptoInfo.userKeyAlgorithm = reader->ReadStringCheck("algorithm", userKeyObject);
        m_cryptoInfo.userKeyCheck = reader->ReadStringCheck("key_check", userKeyObject);

        const rapidjson::Value & signatureObject = reader->ReadObjectCheck("signature", parentObject);//TODO: separate node

        m_cryptoInfo.signatureAlgorithm = reader->ReadStringCheck("algorithm", signatureObject);
        m_cryptoInfo.signatureCertificate = reader->ReadStringCheck("certificate", signatureObject);
        m_cryptoInfo.signature = reader->ReadStringCheck("value", signatureObject);

        BaseLcpNode::ParseNode(encryptionObject, reader);
    }
}