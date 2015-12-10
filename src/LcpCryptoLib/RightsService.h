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

#ifndef __RIGHTS_SERVICE_H__
#define __RIGHTS_SERVICE_H__

#include "Public/IRightsService.h"

namespace lcp
{
    class IRightsManager;
    class IStorageProvider;

    class RightsService : public IRightsService
    {
    public:
        explicit RightsService(IStorageProvider * storageProvider, const std::string & unknownUserId);
        void SyncRightsFromStorage(ILicense * license);

    public:
        // IRightsService
        virtual bool CanUseRight(ILicense * license, const std::string & rightId) const;
        virtual bool UseRight(ILicense * license, const std::string & rightId);
        virtual bool UseRight(ILicense * license, const std::string & rightId, int amount);
        virtual void SetValue(ILicense * license, const std::string & rightId, const std::string & value);
        virtual std::string GetValue(ILicense * license, const std::string & rightId) const;

    private:
        IRightsManager * PerformChecks(ILicense * license) const;
        std::string BuildStorageProviderRightsKeyPrefix(ILicense * license) const;
        std::string BuildStorageProviderRightsKey(ILicense * license, const std::string & rightId) const;
        std::string ExtractRightsKey(const std::string & storageProviderKey) const;

    private:
        IStorageProvider * m_storageProvider;
        std::string m_unknownUserId;
    };
}

#endif //__RIGHTS_SERVICE_H__
