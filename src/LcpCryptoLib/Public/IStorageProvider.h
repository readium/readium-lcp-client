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

#ifndef __I_STORAGE_PROVIDER_H__
#define __I_STORAGE_PROVIDER_H__

#include <string>
#include "IValueIterator.h"

namespace lcp
{
    class IStorageProvider
    {
    public:
        virtual void SetValue(const std::string & vaultId, const std::string & key, const std::string & value) = 0;
        virtual std::string GetValue(const std::string & vaultId, const std::string & key) = 0;
        virtual KvStringsIterator * EnumerateVault(const std::string & vaultId) = 0;
        virtual ~IStorageProvider() {}
    };

    static const char * UserKeysVaultId = "2b741732-f721-4182-9928-b9dcb7edb24e";
    static const char * LicenseRightsVaultId = "8cd95d47-ee95-4f09-b217-621352499d79";
}

#endif //__I_STORAGE_PROVIDER_H__
