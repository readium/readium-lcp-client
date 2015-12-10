//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#ifndef __CERTIFICATE_EXTENSION_H__
#define __CERTIFICATE_EXTENSION_H__

#include <cryptopp/cryptlib.h>
#include <cryptopp/asn.h>
#include "ICertificate.h"

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
