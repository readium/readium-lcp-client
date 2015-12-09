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

#ifndef __I_LCP_LICENSE__
#define __I_LCP_LICENSE__

#include <string>

namespace lcp
{
    class ICrypto;
    class ILinks;
    class IUser;
    class IRights;

    class ILicense
    {
    public:
        // Unique identifier for this license
        virtual std::string Id() const = 0;

        // Canonical form of the license. Used when validating the signature.
        virtual std::string CanonicalContent() const = 0;

        // Original form of the license.
        virtual std::string OriginalContent() const = 0;

        // Date when the license was first issued (ISO 8601).
        virtual std::string Issued() const = 0;

        // Date when the license was last updated (ISO 8601).
        virtual std::string Updated() const = 0;

        // Unique identifier for the Content Provider, as an URI.
        virtual std::string Provider() const = 0;

        virtual ICrypto * Crypto() const = 0;

        // ILinks maps link relation to a Link object, or a list 
        // of Link objects.
        // A link relation is a string such as "publication".
        virtual ILinks * Links() const = 0;

        // Information about the user.
        virtual IUser * User() const = 0;

        // IRights maps a Right identifier to its JSON value, if any.
        virtual IRights * Rights() const = 0;

        // Returns true if the License is decrypted
        virtual bool Decrypted() const = 0;

        virtual ~ILicense() {};
    };
}

#endif //__I_LCP_LICENSE__
