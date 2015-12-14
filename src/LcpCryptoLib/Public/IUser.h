//
//  Created by Mickaël Menu, Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#ifndef __I_LCP_USER_H__
#define __I_LCP_USER_H__

#include "IValueIterator.h"

namespace lcp
{
    //
    // Metadata about the User of a License document.
    //
    class IUser
    {
    public:
        //
        // Unique identifier of the User for the Content Provider.
        //
        virtual std::string Id() const = 0;

        //
        // E-mail address of the user.
        //
        virtual std::string Email() const = 0;

        //
        // Name of the user.
        //
        virtual std::string Name() const = 0;

        //
        // Returns whether the License has the given user info extension.
        //
        virtual bool HasUserValue(const std::string & name) const = 0;

        //
        // Gets the value of the given user extension name.
        // Returns whether the value could be found.
        //
        virtual bool GetUserValue(const std::string & name, std::string & value) const = 0;

        //
        // Enumerates over the User metadata available in the License.
        //
        virtual KvStringsIterator * Enumerate() const = 0;
        
        virtual ~IUser() {}
    };
}

#endif //__I_LCP_USER_H__
