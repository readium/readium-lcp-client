//
//  Created by Mickaël Menu, Artem Brazhnikov on 11/15.
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

#ifndef __I_LCP_CRYPTO_H__
#define __I_LCP_CRYPTO_H__

#include <string>

namespace lcp
{
    class ICrypto
    {
    public:
        // Encryption profile used by this LCP-protected publication,
        // as an URI (e.g. http://readium.org/lcp/profile-1.0)
        virtual std::string EncryptionProfile() const = 0;

        // Content Key encrypted with the User Key (base 64).
        virtual std::string ContentKey() const = 0;

        // Algorithm used to encrypt the Content Key, as an URI (W3C XML
        // Encryption).
        virtual std::string ContentKeyAlgorithm() const = 0;

        // Hint to be displayed to the User to help them remember his
        // passphrase.
        virtual std::string UserKeyHint() const = 0;

        // Algorithm used to generate the user key from the User 
        // Passphrase, as an URI (W3C XML Encryption).
        virtual std::string UserKeyAlgorithm() const = 0;

        // The value of the License's identifier field, encrypted using 
        // the User Key
        virtual std::string UserKeyCheck() const = 0;

        // Value of the License signature.
        virtual std::string Signature() const = 0;

        // Algorithm used to calculate the signature, as an URI (W3C XML
        // Encryption).
        virtual std::string SignatureAlgorithm() const = 0;

        // X509 certificate used by the Content Provider.
        virtual std::string SignatureCertificate() const = 0;

        virtual ~ICrypto() {}
    };
}

#endif //__I_LCP_CRYPTO_H__
