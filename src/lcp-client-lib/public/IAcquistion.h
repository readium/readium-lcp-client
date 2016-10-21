//
//  Created by Mickaël Menu, Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#ifndef __I_ACQUISITION_H__
#define __I_ACQUISITION_H__

#if ENABLE_NET_PROVIDER

#include <string>
#include "LcpStatus.h"

namespace lcp
{
    class IAcquisitionCallback;

    //
    // The IAcquisition controls the download of a protected publication from
    // a standalone LCPL file. Once created using LcpService, it is idle. You
    // must start it and provide an implementation of IAcquisitionCallback
    // to be notified when the download progresses or ends.
    //
    // @see LcpService::CreatePublicationAcquisition()
    class IAcquisition
    {
    public:
        //
        // Starts an idle IAcquisition.
        //
        virtual Status Start(IAcquisitionCallback * callback) = 0;
        
        //
        // Cancels on on-going IAcquisition. The acquisition will be cancelled
        // as soon as it can, and the callback notified.
        //
        virtual Status Cancel() = 0;
        
        //
        // Absolute path to the downloaded publication, when the acquisition
        // ends.
        //
        virtual std::string PublicationPath() const = 0;
        
        //
        // Name suggested by the Content Provider server for the publication
        // file. You can use it to rename the downloaded publication, or
        // ignore it.
        //
        virtual std::string SuggestedFileName() const = 0;
        
        virtual ~IAcquisition() {}
    };
}

#endif //ENABLE_NET_PROVIDER

#endif //__I_ACQUISITION_H__
