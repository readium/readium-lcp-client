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

#include <cryptopp/asn.h>
#include "CertificateRevocationList.h"
#include "CryptoppUtils.h"

using namespace CryptoPP;

namespace lcp
{
    CertificateRevocationList::CertificateRevocationList(const Buffer & crlRaw)
    {
        this->UpdateRevocationList(crlRaw);
    }

    void CertificateRevocationList::UpdateRevocationList(const Buffer & crlRaw)
    {
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
        return m_thisUpdate;
    }

    std::string CertificateRevocationList::NextUpdateDate() const
    {
        return m_nextUpdate;
    }

    bool CertificateRevocationList::HasNextUpdateDate() const
    {
        return !m_nextUpdate.empty();
    }

    bool CertificateRevocationList::SerialNumberRevoked(const std::string & serialNumber) const
    {
        return (m_revokedSerialNumbers.find(serialNumber) != m_revokedSerialNumbers.end());
    }

    const StringsSet & CertificateRevocationList::RevokedSerialNumbers() const
    {
        return m_revokedSerialNumbers;
    }
}