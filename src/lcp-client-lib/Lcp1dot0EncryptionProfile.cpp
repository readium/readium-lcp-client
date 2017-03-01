// Copyright (c) 2016 Mantano
// Licensed to the Readium Foundation under one or more contributor license agreements.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation and/or
//    other materials provided with the distribution.
// 3. Neither the name of the organization nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#include "Lcp1dot0EncryptionProfile.h"
#include "EncryptionProfileNames.h"
#include "AlgorithmNames.h"

#include "LcpUtils.h"

#include "Sha256HashAlgorithm.h"
#include "AesCbcSymmetricAlgorithm.h"
#include "AesGcmSymmetricAlgorithm.h"
#include "RsaSha256SignatureAlgorithm.h"
#include "EcdsaSha256SignatureAlgorithm.h"


namespace lcp
{
    ISymmetricAlgorithm * Lcp1dot0EncryptionProfile::CreatePublicationAlgorithm(
        const KeyType & symmetricKey,
        const std::string & algorithm) const
    {
        //http://www.w3.org/2009/xmlenc11#aes256-gcm
        if (algorithm == AlgorithmNames::AesGcm256Id) {
            return new AesGcmSymmetricAlgorithm(symmetricKey, AesGcmSymmetricAlgorithm::Key256);
        }

        //http://www.w3.org/2001/04/xmlenc#aes256-cbc
        if (algorithm == AlgorithmNames::AesCbc256Id) {
            return new AesCbcSymmetricAlgorithm(symmetricKey, AesCbcSymmetricAlgorithm::Key256);
        }

        throw StatusException(Status(StatusCode::ErrorCommonAlgorithmMismatch, "ErrorCommonAlgorithmMismatch"));
    }

    ISymmetricAlgorithm * Lcp1dot0EncryptionProfile::CreateContentKeyAlgorithm(
        const KeyType & symmetricKey,
        const std::string & algorithm) const
    {
        //http://www.w3.org/2009/xmlenc11#aes256-gcm
        if (algorithm == AlgorithmNames::AesGcm256Id) {
            return new AesGcmSymmetricAlgorithm(symmetricKey, AesGcmSymmetricAlgorithm::Key256);
        }

        //http://www.w3.org/2001/04/xmlenc#aes256-cbc
        if (algorithm == AlgorithmNames::AesCbc256Id) {
            return new AesCbcSymmetricAlgorithm(symmetricKey, AesCbcSymmetricAlgorithm::Key256);
        }

        throw StatusException(Status(StatusCode::ErrorCommonAlgorithmMismatch, "ErrorCommonAlgorithmMismatch"));
    }

    IHashAlgorithm * Lcp1dot0EncryptionProfile::CreateUserKeyAlgorithm() const
    {
        return new Sha256HashAlgorithm();
    }

    ISignatureAlgorithm * Lcp1dot0EncryptionProfile::CreateSignatureAlgorithm(
        const KeyType & publicKey,
        const std::string & algorithm) const
    {
        //http://www.w3.org/2001/04/xmldsig-more#rsa-sha256
        if (algorithm == AlgorithmNames::RsaSha256Id) {
            return new RsaSha256SignatureAlgorithm(publicKey);
        }

        //http://www.w3.org/2001/04/xmldsig-more#ecdsa-sha256
        if (algorithm == AlgorithmNames::EcdsaSha256Id) {
            return new EcdsaSha256SignatureAlgorithm(publicKey);
        }

        throw StatusException(Status(StatusCode::ErrorCommonAlgorithmMismatch, "ErrorCommonAlgorithmMismatch"));
    }

    std::string Lcp1dot0EncryptionProfile::Name() const
    {
        //http://readium.org/lcp/profile-1.0
        return EncryptionProfileNames::Lcp1dot0ProfileId;
    }

    std::string Lcp1dot0EncryptionProfile::UserKeyAlgorithm() const
    {
        //http://www.w3.org/2001/04/xmlenc#sha256
        return AlgorithmNames::Sha256Id;
    }

    std::string Lcp1dot0EncryptionProfile::PublicationAlgorithmCBC() const
    {
        //http://www.w3.org/2001/04/xmlenc#aes256-cbc
        return AlgorithmNames::AesCbc256Id;
    }

    std::string Lcp1dot0EncryptionProfile::PublicationAlgorithmGCM() const
    {
        //http://www.w3.org/2009/xmlenc11#aes256-gcm
        return AlgorithmNames::AesGcm256Id;
    }

    std::string Lcp1dot0EncryptionProfile::ContentKeyAlgorithmCBC() const
    {
        //http://www.w3.org/2001/04/xmlenc#aes256-cbc
        return AlgorithmNames::AesCbc256Id;
    }

    std::string Lcp1dot0EncryptionProfile::ContentKeyAlgorithmGCM() const
    {
        //http://www.w3.org/2009/xmlenc11#aes256-gcm
        return AlgorithmNames::AesGcm256Id;
    }

    std::string Lcp1dot0EncryptionProfile::SignatureAlgorithmRSA() const
    {
        //http://www.w3.org/2001/04/xmldsig-more#rsa-sha256
        return AlgorithmNames::RsaSha256Id;
    }

    std::string Lcp1dot0EncryptionProfile::SignatureAlgorithmECDSA() const
    {
        //http://www.w3.org/2001/04/xmldsig-more#ecdsa-sha256
        return AlgorithmNames::EcdsaSha256Id;
    }
}