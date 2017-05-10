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
//        virtual bool VerifyMessage(const std::string & message, const std::string & hashBase64) = 0;
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

#if !DISABLE_CRL

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
        virtual const void InsertRevokedSerialNumber(std::string serial) = 0;
        virtual ~ICertificateRevocationList() {}
    };

#endif //!DISABLE_CRL

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
