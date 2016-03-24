//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#ifndef __CERTIFICATE_EXTENSION_H__
#define __CERTIFICATE_EXTENSION_H__

#include "IncludeMacros.h"
#include "ICertificate.h"

CRYPTOPP_INCLUDE_START
#include <cryptopp/cryptlib.h>
#include <cryptopp/asn.h>
CRYPTOPP_INCLUDE_END

namespace lcp
{
    class CertificateExtension : public ICertificateExtension
    {
    public:
        explicit CertificateExtension(CryptoPP::BERSequenceDecoder & parent);
        CertificateExtension(const CryptoPP::OID & oid, bool isCritical, const Buffer & value);
        void Decode(CryptoPP::BERSequenceDecoder & parent);
        CryptoPP::OID CryptoOid() const;

        // ICertificateExtension
        virtual std::string Oid() const;
        virtual bool IsCritical() const;
        virtual const Buffer & Value() const;

    private:
        CryptoPP::OID m_extensionId;
        bool m_isCritical;
        Buffer m_extensionValue;
    };
}

#endif //__CERTIFICATE_EXTENSION_H__
