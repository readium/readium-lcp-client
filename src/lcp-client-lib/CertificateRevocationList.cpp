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


#if ENABLE_NET_PROVIDER

#include "CertificateRevocationList.h"
#include "CryptoppUtils.h"
#include "IncludeMacros.h"

CRYPTOPP_INCLUDE_START
#include <cryptopp/asn.h>
CRYPTOPP_INCLUDE_END

using namespace CryptoPP;

namespace lcp
{
    CertificateRevocationList::CertificateRevocationList(const Buffer & crlRaw)
    {
        this->UpdateRevocationList(crlRaw);
    }

    void CertificateRevocationList::UpdateRevocationList(const Buffer & crlRaw)
    {
        std::unique_lock<std::mutex> locker(m_sync);

        ByteQueue crlData;
        crlData.Put(crlRaw.data(), crlRaw.size());
        crlData.MessageEnd();

        BERSequenceDecoder crl(crlData);
        {
            BERSequenceDecoder toBeSignedCertList(crl);
            {
                word32 version = CryptoppUtils::Cert::ReadVersion(toBeSignedCertList, CertificateVersion::Certificatev2);
                if (version != CertificateVersion::Certificatev2)
                {
                    throw BERDecodeErr("Wrong version of the crl");
                }

                // algorithmId
                CryptoppUtils::Cert::SkipNextSequence(toBeSignedCertList);
                // issuer
                CryptoppUtils::Cert::SkipNextSequence(toBeSignedCertList);
                // this update
                CryptoppUtils::Cert::BERDecodeTime(toBeSignedCertList, m_thisUpdate);
                // next update
                if (!toBeSignedCertList.EndReached())
                {
                    byte nextId = toBeSignedCertList.PeekByte();
                    if (nextId == UTC_TIME || nextId == GENERALIZED_TIME)
                    {
                        CryptoppUtils::Cert::BERDecodeTime(toBeSignedCertList, m_nextUpdate);
                    }
                }

                if (!toBeSignedCertList.EndReached())
                {
                    BERSequenceDecoder revokedCertificates(toBeSignedCertList);
                    {
                        while (!revokedCertificates.EndReached())
                        {
                            BERSequenceDecoder nextRevokedCertificate(revokedCertificates);
                            {
                                std::string serial = CryptoppUtils::Cert::ReadIntegerAsString(nextRevokedCertificate);
                                m_revokedSerialNumbers.insert(serial);
                            }
                            nextRevokedCertificate.SkipAll();
                        }
                    }
                }
                toBeSignedCertList.SkipAll();
            }
        }
    }

    std::string CertificateRevocationList::ThisUpdateDate() const
    {
        std::unique_lock<std::mutex> locker(m_sync);
        return m_thisUpdate;
    }

    bool CertificateRevocationList::HasThisUpdateDate() const
    {
        std::unique_lock<std::mutex> locker(m_sync);
        return !m_thisUpdate.empty();
    }

    std::string CertificateRevocationList::NextUpdateDate() const
    {
        std::unique_lock<std::mutex> locker(m_sync);
        return m_nextUpdate;
    }

    bool CertificateRevocationList::HasNextUpdateDate() const
    {
        std::unique_lock<std::mutex> locker(m_sync);
        return !m_nextUpdate.empty();
    }

    bool CertificateRevocationList::SerialNumberRevoked(const std::string & serialNumber) const
    {
        std::unique_lock<std::mutex> locker(m_sync);
        return (m_revokedSerialNumbers.find(serialNumber) != m_revokedSerialNumbers.end());
    }

    const StringsSet & CertificateRevocationList::RevokedSerialNumbers() const
    {
        std::unique_lock<std::mutex> locker(m_sync);
        return m_revokedSerialNumbers;
    }
}

#endif //ENABLE_NET_PROVIDER
