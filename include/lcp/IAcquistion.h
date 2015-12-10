//
//  Created by Mickaël Menu, Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#ifndef __I_ACQUISITION_H__
#define __I_ACQUISITION_H__

#include <string>
#include "LcpStatus.h"

namespace lcp
{
    class IAcquisitionCallback;

    class IAcquisition
    {
    public:
        virtual Status Start(IAcquisitionCallback * callback) = 0;
        virtual void Cancel() = 0;
        virtual std::string PublicationPath() const = 0;
        virtual std::string SuggestedFileName() const = 0;
        virtual ~IAcquisition() {}
    };
}

#endif //__I_ACQUISITION_H__
