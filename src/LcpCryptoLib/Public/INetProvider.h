//
//  Created by Mickaël Menu, Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#ifndef __I_NET_PROVIDER_H__
#define __I_NET_PROVIDER_H__

#include <string>
#include "LcpStatus.h"

namespace lcp
{
    class IWritableStream;

    class INetRequest
    {
    public:
        virtual std::string Url() const = 0;
        virtual bool Canceled() const = 0;
        virtual void SetCanceled(bool value) = 0;
        virtual ~INetRequest() {}
    };

    class IDownloadRequest : public INetRequest
    {
    public:
        // INetProvider writes downloaded data into the stream,
        // may not be used if INetProvider uses the DestinationPath
        virtual IWritableStream * DestinationStream() const = 0;
        // Returns whether IDownloadRequest uses path or not
        virtual bool HasDesinationPath() const = 0;
        // Returns the download path, may be used instead of the DestinationStream
        virtual std::string DestinationPath() const = 0;
        virtual std::string SuggestedFileName() const = 0;
        virtual void SetSuggestedFileName(const std::string & fileName) = 0;
    };

    class INetProviderCallback
    {
    public:
        virtual void OnRequestStarted(INetRequest * request) = 0;
        virtual void OnRequestProgressed(INetRequest * request, float progress) = 0;
        virtual void OnRequestCanceled(INetRequest * request) = 0;
        virtual void OnRequestEnded(INetRequest * request, Status result) = 0;
        virtual ~INetProviderCallback() {}
    };

    class INetProvider
    {
    public:
        // Asynchronous network request
        virtual void StartDownloadRequest(
            IDownloadRequest * request,
            INetProviderCallback * callback
            ) = 0;
        virtual ~INetProvider() {}
    };
}

#endif //__I_NET_PROVIDER_H__
