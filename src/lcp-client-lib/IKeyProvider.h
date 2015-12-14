//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#ifndef __I_KEY_PROVIDER_H__
#define __I_KEY_PROVIDER_H__

#include "LcpTypedefs.h"

namespace lcp
{
    class IKeyProvider
    {
    public:
        virtual KeyType UserKey() const = 0;
        virtual KeyType ContentKey() const = 0;
        virtual ~IKeyProvider() {}
    };
}

#endif //__I_KEY_PROVIDER_H__
