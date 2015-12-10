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
    class IUser
    {
    public:
        virtual std::string Id() const = 0;
        virtual std::string Email() const = 0;
        virtual std::string Name() const = 0;
        virtual bool HasUserValue(const std::string & name) const = 0;
        virtual bool GetUserValue(const std::string & name, std::string & value) const = 0;
        virtual KvStringsIterator * Enumerate() const = 0;
        virtual ~IUser() {}
    };
}

#endif //__I_LCP_USER_H__
