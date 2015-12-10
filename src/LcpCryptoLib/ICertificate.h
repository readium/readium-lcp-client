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

#ifndef __I_CERTIFICATE_H__
#define __I_CERTIFICATE_H__

#include <string>
#include <vector>
#include "LcpUtils.h"

namespace lcp
{
    class ICertificate
    {
    public:
        virtual std::string SerialNumber() const = 0;
        virtual std::string NotBeforeDate() const = 0;
        virtual std::string NotAfterDate() const = 0;
        virtual KeyType PublicKey() const = 0;
        virtual bool VerifyCertificate(ICertificate * rootCertificate) = 0;
        virtual bool VerifyMessage(const std::string & message, const std::string & hashBase64) = 0;
        virtual ~ICertificate() {}
    };
}

#endif //__I_CERTIFICATE_H__
