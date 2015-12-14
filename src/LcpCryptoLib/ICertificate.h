//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#ifndef __I_CERTIFICATE_H__
#define __I_CERTIFICATE_H__

#include "LcpTypedefs.h"

namespace lcp
{
    class ICrlDistributionPoints
    {
    public:
        virtual bool HasCrlDistributionPoints() const = 0;
        virtual const StringsList & CrlDistributionPointUrls() const = 0;
        virtual ~ICrlDistributionPoints() {}
    };

    class ICertificate
    {
    public:
        virtual std::string SerialNumber() const = 0;
        virtual std::string NotBeforeDate() const = 0;
        virtual std::string NotAfterDate() const = 0;
        virtual KeyType PublicKey() const = 0;
        virtual bool VerifyCertificate(ICertificate * rootCertificate) = 0;
        virtual bool VerifyMessage(const std::string & message, const std::string & hashBase64) = 0;
        virtual ICrlDistributionPoints * DistributionPoints() const = 0;
        virtual ~ICertificate() {}
    };

    class ICertificateExtension
    {
    public:
        virtual std::string Oid() const = 0;
        virtual bool IsCritical() const = 0;
        virtual const Buffer & Value() const = 0;
        virtual ~ICertificateExtension() {}
    };

    class ICertificateRevocationList
    {
    public:
        virtual void UpdateRevocationList(const Buffer & crlRaw) = 0;
        virtual bool HasThisUpdateDate() const = 0;
        virtual std::string ThisUpdateDate() const = 0;
        virtual bool HasNextUpdateDate() const = 0;
        virtual std::string NextUpdateDate() const = 0;
        virtual bool SerialNumberRevoked(const std::string & serialNumber) const = 0;
        virtual const StringsSet & RevokedSerialNumbers() const = 0;
        virtual ~ICertificateRevocationList() {}
    };

    struct CertificateVersion
    {
        enum CertificateVersionEnum
        {
            Certificatev1 = 0,
            Certificatev2 = 1,
            Certificatev3 = 2
        };
    };
}

#endif //__I_CERTIFICATE_H__
