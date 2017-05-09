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


#ifndef __CERTIFICATE_H__
#define __CERTIFICATE_H__

#include "CertificateExtension.h"
#include "CrlDistributionPoints.h"
#include "IncludeMacros.h"
#include "ICertificate.h"
#include "LcpTypedefs.h"
#include "NonCopyable.h"
#include <string>

CRYPTOPP_INCLUDE_START
#include <cryptopp/eccrypto.h>
#include <cryptopp/rsa.h>
#include <cryptopp/secblock.h>
CRYPTOPP_INCLUDE_END

using namespace CryptoPP;

namespace lcp
{
    class IEncryptionProfile;
    class ISignatureAlgorithm;

    class Certificate : public ICertificate, public NonCopyable
    {
    public:
        explicit Certificate(
            const std::string & certificateBase64,
            IEncryptionProfile * encryptionProfile
            );

        std::string SerialNumber() const;
        std::string NotBeforeDate() const;
        std::string NotAfterDate() const;
        KeyType PublicKey() const;

        bool VerifyCertificate(ICertificate * rootCertificate);
        bool VerifyMessage(const std::string & message, const std::string & hashBase64);
        bool VerifyMessage(
            const unsigned char * message,
            size_t messageLength,
            const unsigned char * signature,
            size_t signatureLength
            );

        ICrlDistributionPoints * DistributionPoints() const;

    private:
        std::string m_serialNumber;
        std::string m_notBeforeDate;
        std::string m_notAfterDate;

//        CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PublicKey m_publicKeyECDSA;
////        CryptoPP::DL_PublicKey_EC<CryptoPP::ECP> m_publicKeyECDSA;
//        CryptoPP::RSA::PublicKey m_publicKeyRSA;
//        ByteQueue m_publicKeyQueue;
        KeyType m_publicKeyType;

        SecByteBlock m_toBeSignedData;
        SecByteBlock m_rootSignature;
        OID m_signatureAlgorithmId;

        std::vector<CertificateExtension> m_extensions;
        std::unique_ptr<CrlDistributionPoints> m_distributionPoints;

        IEncryptionProfile * m_encryptionProfile;
        std::unique_ptr<ISignatureAlgorithm> m_signatureAlgorithm;
    };
}

#endif //__CERTIFICATE_H__
