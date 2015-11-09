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

    Status CryptoLcpNode::ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader)
    {
        const rapidjson::Value & encryptionObject = reader->ReadAsObjectCheck("encryption", parentObject);

        m_cryptoInfo.encryptionProfile = reader->ReadAsStringCheck("profile", encryptionObject);

        const rapidjson::Value & contentKeyObject = reader->ReadAsObjectCheck("content_key", encryptionObject);

        m_cryptoInfo.contentKey = reader->ReadAsStringCheck("encrypted_value", contentKeyObject);
        m_cryptoInfo.contentKeyAlgorithm = reader->ReadAsStringCheck("algorithm", contentKeyObject);

        const rapidjson::Value & userKeyObject = reader->ReadAsObjectCheck("user_key", encryptionObject);

        m_cryptoInfo.userKeyHint = reader->ReadAsStringCheck("text_hint", userKeyObject);
        m_cryptoInfo.userKeyAlgorithm = reader->ReadAsStringCheck("algorithm", userKeyObject);
        m_cryptoInfo.userKeyCheck = reader->ReadAsStringCheck("key_check", userKeyObject);

        const rapidjson::Value & signatureObject = reader->ReadAsObjectCheck("signature", parentObject);//TODO: separate node

        m_cryptoInfo.signatureAlgorithm = reader->ReadAsStringCheck("algorithm", signatureObject);
        m_cryptoInfo.signatureCertificate = reader->ReadAsStringCheck("certificate", signatureObject);
        m_cryptoInfo.signature = reader->ReadAsStringCheck("value", signatureObject);

        return Status(StCodeCover::ErrorSuccess);
    }
}