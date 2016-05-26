// Copyright (c) 2016 Mantano
// Licensed to the Readium Foundation under one or more contributor license agreements.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation and/or
//    other materials provided with the distribution.
// 3. Neither the name of the organization nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.



#ifndef __I_ACQUISTION_CALLBACK_H__
#define __I_ACQUISTION_CALLBACK_H__

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

#endif //__I_ACQUISTION_CALLBACK_H__
