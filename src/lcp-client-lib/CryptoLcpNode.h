//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
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
        virtual void ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader);
        virtual Status VerifyNode(ILicense * license, IClientProvider * clientProvider, ICryptoProvider * cryptoProvider);
        virtual Status DecryptNode(ILicense * license, IKeyProvider * keyProvider, ICryptoProvider * cryptoProvider);

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
