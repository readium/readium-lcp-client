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

#ifndef __RSA_SHA256_SIGNATURE_ALGORITHM_H__
#define __RSA_SHA256_SIGNATURE_ALGORITHM_H__

#include <cryptopp/rsa.h>
#include "CryptoAlgorithmInterfaces.h"

namespace lcp
{
    class RsaSha256SignatureAlgorithm : public ISignatureAlgorithm
    {
    public:
        explicit RsaSha256SignatureAlgorithm(const KeyType & publicKey);

        virtual std::string Name() const;

        virtual bool VerifySignature(
            const std::string & message,
            const std::string & signatureBase64
            );

        virtual bool VerifySignature(
            const unsigned char * message,
            size_t messageLength,
            const unsigned char * signature,
            size_t signatureLength
            );

    private:
        typedef CryptoPP::RSASS<CryptoPP::PKCS1v15, CryptoPP::SHA256>::Verifier ThisVerifier;
        CryptoPP::RSA::PublicKey m_publicKey;
    };
}

#endif //__RSA_SHA256_SIGNATURE_ALGORITHM_H__
