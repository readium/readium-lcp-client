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