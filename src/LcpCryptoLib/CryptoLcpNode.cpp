#include "rapidjson/document.h"
#include "CryptoLcpNode.h"
#include "JsonValueReader.h"

namespace lcp
{
    std::string CryptoLcpNode::EncryptionProfile() const
    {
        return m_cryptoInfo.EncryptionProfile;
    }

    std::string CryptoLcpNode::ContentKey() const
    {
        return m_cryptoInfo.ContentKey;
    }

    std::string CryptoLcpNode::ContentKeyAlgorithm() const
    {
        return m_cryptoInfo.ContentKeyAlgorithm;
    }

    std::string CryptoLcpNode::UserKeyHint() const
    {
        return m_cryptoInfo.UserKeyHint;
    }

    std::string CryptoLcpNode::UserKeyAlgorithm() const
    {
        return m_cryptoInfo.UserKeyAlgorithm;
    }

    std::string CryptoLcpNode::UserKeyCheck() const
    {
        return m_cryptoInfo.UserKeyCheck;
    }

    std::string CryptoLcpNode::Signature() const
    {
        return m_cryptoInfo.Signature;
    }

    std::string CryptoLcpNode::SignatureAlgorithm() const
    {
        return m_cryptoInfo.SignatureAlgorithm;
    }

    std::string CryptoLcpNode::SignatureCertificate() const
    {
        return m_cryptoInfo.SignatureCertificate;
    }

    Status CryptoLcpNode::ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader)
    {
        const rapidjson::Value & encryptionObject = reader->ReadAsObjectCheck("encryption", parentObject);

        m_cryptoInfo.EncryptionProfile = reader->ReadAsStringCheck("profile", encryptionObject);

        const rapidjson::Value & contentKeyObject = reader->ReadAsObjectCheck("content_key", encryptionObject);

        m_cryptoInfo.ContentKey = reader->ReadAsStringCheck("encrypted_value", contentKeyObject);
        m_cryptoInfo.ContentKeyAlgorithm = reader->ReadAsStringCheck("algorithm", contentKeyObject);

        const rapidjson::Value & userKeyObject = reader->ReadAsObjectCheck("user_key", encryptionObject);

        m_cryptoInfo.UserKeyHint = reader->ReadAsStringCheck("text_hint", userKeyObject);
        m_cryptoInfo.UserKeyAlgorithm = reader->ReadAsStringCheck("algorithm", userKeyObject);
        m_cryptoInfo.UserKeyCheck = reader->ReadAsStringCheck("key_check", userKeyObject);

        const rapidjson::Value & signatureObject = reader->ReadAsObjectCheck("signature", parentObject);//TODO: separate node

        m_cryptoInfo.SignatureAlgorithm = reader->ReadAsStringCheck("algorithm", signatureObject);
        m_cryptoInfo.SignatureCertificate = reader->ReadAsStringCheck("certificate", signatureObject);
        m_cryptoInfo.Signature = reader->ReadAsStringCheck("value", signatureObject);

        return Status(StCodeCover::ErrorSuccess);
    }
}