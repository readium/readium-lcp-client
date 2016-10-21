//
//  Created by Mickaël Menu, Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#ifndef __I_ACQUISTION_CALLBACK_H__
#define __I_ACQUISTION_CALLBACK_H__

#if ENABLE_NET_PROVIDER

#include "LcpStatus.h"

namespace lcp
{
    class IAcquisition;

    //
    // Implement this interface to be notified when a given IAcquisition
    // changes.
    //
    class IAcquisitionCallback
    {
    public:
        //
        // Called before the request is made to the Content Provider server.
        //
        virtual void OnAcquisitionStarted(IAcquisition * acquisition) = 0;
        
        //
        // Called when receiving data from the Content Provider server. The
        // progress should be used to provide feedback to the user.
        //
        virtual void OnAcquisitionProgressed(IAcquisition * acquisition, float progress) = 0;
        
        //
        // Called after the acquisition was cancelled using
        // IAcquisition::Cancel(), and the network request interrupted.
        //
        virtual void OnAcquisitionCanceled(IAcquisition * acquisition) = 0;
        
        //
        // Called when the acquisition ends. You must check the Status to know
        // if the publication was downloaded, or if an error occurred.
        // Use IAcquisition::PublicationPath() to get the publication.
        //
        virtual void OnAcquisitionEnded(IAcquisition * acquisition, Status result) = 0;
        
        virtual ~IAcquisitionCallback() {}
    };
}

#endif //ENABLE_NET_PROVIDER

#endif //__I_ACQUISTION_CALLBACK_H__
