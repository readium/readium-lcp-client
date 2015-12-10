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

#include <sstream>
#include "CertificateExtension.h"

namespace lcp
{
    CertificateExtension::CertificateExtension(CryptoPP::BERSequenceDecoder & parent)
    {
        this->Decode(parent);
    }

    CertificateExtension::CertificateExtension(const CryptoPP::OID & oid, const bool isCritical, const Buffer & value)
        : m_extensionId(oid), m_isCritical(isCritical), m_extensionValue(value)
    {
    }

    void CertificateExtension::Decode(CryptoPP::BERSequenceDecoder & parent)
    {
        CryptoPP::BERSequenceDecoder extension(parent);
        {
            m_extensionId.BERDecode(extension);
            m_isCritical = false;
            byte booleanTag = extension.PeekByte();
            if (booleanTag == CryptoPP::BOOLEAN)
            {
                CryptoPP::BERDecodeUnsigned(extension, m_isCritical, CryptoPP::BOOLEAN);
            }
            CryptoPP::SecByteBlock secBuffer;
            CryptoPP::BERDecodeOctetString(extension, secBuffer);
            m_extensionValue.assign(secBuffer.begin(), secBuffer.end());
        }
        extension.MessageEnd();
    }

    CryptoPP::OID CertificateExtension::CryptoOid() const
    {
        return m_extensionId;
    }

    std::string CertificateExtension::Oid() const
    {
        std::stringstream oid;
        for (auto val : m_extensionId.m_values)
        {
            oid << val << ".";
        }
        std::string oidStr = oid.str();
        return oidStr.substr(0, oidStr.find_last_of("."));
    }

    bool CertificateExtension::IsCritical() const
    {
        return m_isCritical;
    }

    const Buffer & CertificateExtension::Value() const
    {
        return m_extensionValue;
    }
}