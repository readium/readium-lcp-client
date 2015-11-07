#ifndef __ENCRYPTION_LCP_NODE_H__
#define __ENCRYPTION_LCP_NODE_H__

#include "BaseLcpNode.h"
#include "ICrypto.h"

namespace lcp
{
    struct CryptoInfo
    {
        std::string EncryptionProfile;
        std::string ContentKey;
        std::string ContentKeyAlgorithm;
        std::string UserKeyHint;
        std::string UserKeyAlgorithm;
        std::string UserKeyCheck;
        std::string Signature;
        std::string SignatureAlgorithm;
        std::string SignatureCertificate;
    };

    class CryptoLcpNode : public BaseLcpNode, public ICrypto
    {
    public:
        // ILcpNode
        Status ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader);

    public:
        // ICrypto
        std::string EncryptionProfile() const;
        std::string ContentKey() const;
        std::string ContentKeyAlgorithm() const;
        std::string UserKeyHint() const;
        std::string UserKeyAlgorithm() const;
        std::string UserKeyCheck() const;
        std::string Signature() const;
        std::string SignatureAlgorithm() const;
        std::string SignatureCertificate() const;

    private:
        CryptoInfo m_cryptoInfo;
    };
}
#endif //__ENCRYPTION_LCP_NODE_H__
