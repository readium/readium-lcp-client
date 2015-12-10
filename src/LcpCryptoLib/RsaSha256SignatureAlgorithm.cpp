//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//  Licensed under Gnu Affero General Public License Version 3 (provided, notwithstanding this notice,
//  Readium Foundation reserves the right to license this material under a different separate license,
//  and if you have done so, the terms of that separate license control and the following references
//  to GPL do not apply).
//
//  This program is free software: you can redistribute it and/or modify it under the terms of the GNU
//  Affero General Public License as published by the Free Software Foundation, either version 3 of
//  the License, or (at your option) any later version. You should have received a copy of the GNU
//  Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
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