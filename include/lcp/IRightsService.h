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

#ifndef __I_RIGHTS_SERVICE_H__
#define __I_RIGHTS_SERVICE_H__

#include <string>

namespace lcp
{
    class ILicense;

    class IRightsService
    {
    public:
        // Returns whether the user can use the given right. Only works with
        // the rights identifiers defined in the specification.
        // Eg. for "copy", returns whether the User can still copy text
        // from the publication.
        // For "end", returns whether the License isn't expired.
        // If the right identifier can't be found in the License, should
        // return true.
        virtual bool CanUseRight(ILicense * license, const std::string & rightId) const = 0;

        // Consumes one or more right units. Only works with rights with
        // a set amount, e.g. copy and print. If no amount is given, assume 1 unit.
        // if units count is not enough return false
        // Only relevant for rights that can be consumed, like Copy and Print.
        virtual bool UseRight(ILicense * license, const std::string & rightId) = 0;
        virtual bool UseRight(ILicense * license, const std::string & rightId, int amount) = 0;

        // Stores the given JSON value in the secure storage for the 
        // given license and right identifier. It can be used by Clients 
        // to implement custom rights.
        virtual void SetValue(ILicense * license, const std::string & rightId, const std::string & value) = 0;

        // Retrieves the current JSON value in the storage provider 
        // associated with the given license and right identifier. It can
        // be used by Clients to implement custom rights.
        virtual std::string GetValue(ILicense * license, const std::string & rightId) const = 0;

        virtual ~IRightsService() {}
        
        static int UNLIMITED;
    };
}

#endif //__I_RIGHTS_SERVICE_H__
