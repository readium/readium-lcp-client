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

#include "Lcp1dot0EncryptionProfile.h"
#include "EncryptionProfileNames.h"
#include "AlgorithmNames.h"

#include "Sha256HashAlgorithm.h"
#include "AesCbcSymmetricAlgorithm.h"
#include "RsaSha256SignatureAlgorithm.h"

namespace lcp
{
    ISymmetricAlgorithm * Lcp1dot0EncryptionProfile::CreatePublicationAlgorithm(
        const KeyType & symmetricKey) const
    {
        return new AesCbcSymmetricAlgorithm(symmetricKey, AesCbcSymmetricAlgorithm::Key256);
    }

    ISymmetricAlgorithm * Lcp1dot0EncryptionProfile::CreateContentKeyAlgorithm(
        const KeyType & symmetricKey) const
    {
        return new AesCbcSymmetricAlgorithm(symmetricKey, AesCbcSymmetricAlgorithm::Key256);
    }

    IHashAlgorithm * Lcp1dot0EncryptionProfile::CreateUserKeyAlgorithm() const
    {
        return new Sha256HashAlgorithm();
    }

    ISignatureAlgorithm * Lcp1dot0EncryptionProfile::CreateSignatureAlgorithm(
        const KeyType & publicKey) const
    {
        return new RsaSha256SignatureAlgorithm(publicKey);
    }

    std::string Lcp1dot0EncryptionProfile::Name() const
    {
        return EncryptionProfileNames::Lcp1dot0ProfileId;
    }

    std::string Lcp1dot0EncryptionProfile::UserKeyAlgorithm() const
    {
        return AlgorithmNames::Sha256Id;
    }

    std::string Lcp1dot0EncryptionProfile::PublicationAlgorithm() const
    {
        return AlgorithmNames::AesCbc256Id;
    }

    std::string Lcp1dot0EncryptionProfile::ContentKeyAlgorithm() const
    {
        return AlgorithmNames::AesCbc256Id;
    }

    std::string Lcp1dot0EncryptionProfile::SignatureAlgorithm() const
    {
        return AlgorithmNames::RsaSha256Id;
    }
}