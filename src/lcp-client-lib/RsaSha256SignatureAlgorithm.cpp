//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#include "RsaSha256SignatureAlgorithm.h"
#include "AlgorithmNames.h"
#include "CryptoppUtils.h"

namespace lcp
{
    RsaSha256SignatureAlgorithm::RsaSha256SignatureAlgorithm(const KeyType & publicKey)
    {
        ByteQueue publicKeyQueue;
        KeyType rawPublicKey = publicKey;
        publicKeyQueue.Put(&rawPublicKey.at(0), rawPublicKey.size());
        publicKeyQueue.MessageEnd();
        m_publicKey.BERDecode(publicKeyQueue);
    }

    std::string RsaSha256SignatureAlgorithm::Name() const
    {
        return AlgorithmNames::RsaSha256Id;
    }

    bool RsaSha256SignatureAlgorithm::VerifySignature(
        const std::string & message,
        const std::string & signatureBase64
        )
    {
        CryptoPP::SecByteBlock rawSignature;
        CryptoppUtils::Base64ToSecBlock(signatureBase64, rawSignature);

        return this->VerifySignature(
            reinterpret_cast<const byte *>(message.data()),
            message.size(),
            rawSignature.data(),
            rawSignature.size()
            );
    }

    bool RsaSha256SignatureAlgorithm::VerifySignature(
        const unsigned char * message,
        size_t messageLength,
        const unsigned char * signature,
        size_t signatureLength
        )
    {
        ThisVerifier verifier(m_publicKey);
        return verifier.VerifyMessage(
            message,
            messageLength,
            signature,
            signatureLength
            );
    }
}