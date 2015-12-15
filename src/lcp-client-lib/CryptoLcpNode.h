//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#ifndef __ENCRYPTION_LCP_NODE_H__
#define __ENCRYPTION_LCP_NODE_H__

#include "BaseLcpNode.h"
#include "public/ICrypto.h"

namespace lcp
{
    class IEncryptionProfile;
    class EncryptionProfilesManager;

    struct CryptoInfo
    {
        std::string encryptionProfile;
        std::string contentKey;
        std::string contentKeyAlgorithm;
        std::string userKeyHint;
        std::string userKeyAlgorithm;
        std::string userKeyCheck;
        std::string signature;
        std::string signatureAlgorithm;
        std::string signatureCertificate;
    };

    class CryptoLcpNode : public BaseLcpNode, public ICrypto
    {
    public:
        explicit CryptoLcpNode(EncryptionProfilesManager * encryptionProfilesManager);

        // ILcpNode
        void ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader);
        Status VerifyNode(ILicense * license, IClientProvider * clientProvider, ICryptoProvider * cryptoProvider);

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
        IEncryptionProfile * m_encryptionProfile;
        EncryptionProfilesManager * m_encryptionProfilesManager;
    };
}
#endif //__ENCRYPTION_LCP_NODE_H__
