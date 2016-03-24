//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
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