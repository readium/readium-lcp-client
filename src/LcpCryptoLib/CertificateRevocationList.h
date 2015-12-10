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

#ifndef __CERTIFICATE_REVOCATION_LIST_H__
#define __CERTIFICATE_REVOCATION_LIST_H__

#include "ICertificate.h"

namespace lcp
{
    class CertificateRevocationList : public ICertificateRevocationList
    {
    public:
        CertificateRevocationList() = default;
        explicit CertificateRevocationList(const Buffer & crlRaw);
        
        // ICertificateRevocationList
        virtual void UpdateRevocationList(const Buffer & crlRaw);
        virtual std::string ThisUpdateDate() const;
        virtual bool HasNextUpdateDate() const;
        virtual std::string NextUpdateDate() const;
        virtual bool SerialNumberRevoked(const std::string & serialNumber) const;
        virtual const StringsSet & RevokedSerialNumbers() const;

    private:
        std::string m_thisUpdate;
        std::string m_nextUpdate;
        StringsSet m_revokedSerialNumbers;
    };
}

#endif //__CERTIFICATE_REVOCATION_LIST_H__
