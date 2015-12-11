//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#ifndef __CERTIFICATE_H__
#define __CERTIFICATE_H__

#include <string>
#include <cryptopp/rsa.h>
#include <cryptopp/secblock.h>
#include "ICertificate.h"
#include "CertificateExtension.h"
#include "CrlDistributionPoints.h"

using namespace CryptoPP;

namespace lcp
{
    class IEncryptionProfile;
    class ISignatureAlgorithm;

    class Certificate : public ICertificate
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
        RSA::PublicKey m_publicKey;
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
