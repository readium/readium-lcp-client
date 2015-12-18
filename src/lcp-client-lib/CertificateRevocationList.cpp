//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

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
                BERDecodeTime(toBeSignedCertList, m_thisUpdate);
                // next update
                if (!toBeSignedCertList.EndReached())
                {
                    byte nextId = toBeSignedCertList.PeekByte();
                    if (nextId == UTC_TIME || nextId == GENERALIZED_TIME)
                    {
                        BERDecodeTime(toBeSignedCertList, m_nextUpdate);
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