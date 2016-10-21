//
//  Created by Mickaël Menu, Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#ifndef __I_NET_PROVIDER_H__
#define __I_NET_PROVIDER_H__

#if ENABLE_NET_PROVIDER

#include <string>
#include "LcpStatus.h"

namespace lcp
{
    class INetProviderCallback;
    class IDownloadRequest;
    class INetRequest;
    class IWritableStream;

    //
    // Interface to be implemented by the client to handle any HTTP call
    // for the library.
    // It is used, for example, to download protected publications or get the
    // certificate revocation list.
    //
    class INetProvider
    {
    public:
        //
        // Starts the given download request.
        // The implementer must start an HTTP request to the given request's
        // URL. The given callback will be used to notify the caller when
        // the request progresses.
        // The implementation should be asynchronous.
        //
        virtual void StartDownloadRequest(
            IDownloadRequest * request,
            INetProviderCallback * callback
            ) = 0;

        //
        // The implementer should cancel the request
        // The implementation should be asynchronous
        virtual void CancelDownloadRequest(IDownloadRequest * request) {};

        virtual ~INetProvider() {}
    };

    // 
    // Callback interface used by the net provider to notify the progress and
    // status of a net request.
    //
    class INetProviderCallback
    {
    public:
        virtual void OnRequestStarted(INetRequest * request) = 0;
        virtual void OnRequestProgressed(INetRequest * request, float progress) = 0;
        virtual void OnRequestCanceled(INetRequest * request) = 0;
        virtual void OnRequestEnded(INetRequest * request, Status result) = 0;
        virtual ~INetProviderCallback() {}
    };

    //
    // Abstract class representing a single HTTP request.
    //
    class INetRequest
    {
    public:
        virtual std::string Url() const = 0;
        virtual bool Canceled() const = 0;
        virtual void SetCanceled(bool value) = 0;
        virtual ~INetRequest() {}
    };

    //
    // A special kind of net request used to download a file through HTTP.
    //
    class IDownloadRequest : public INetRequest
    {
    public:
        //
        // By default, you should use this stream to write the downloaded data.
        // As an alternative, you can use the DestinationPath to write the file
        // directly, if available.
        //
        virtual IWritableStream * DestinationStream() const = 0;

        //
        // Returns whether this IDownloadRequest uses a local file for
        // destination. If true, you can use the destination path to write
        // the downloaded data directly instead of using the DestinationStream.
        virtual bool HasDestinationPath() const = 0;

        //
        // Returns the absolute path to the file to download, if any.
        // This might be null.
        // 
        virtual std::string DestinationPath() const = 0;

        //
        // Suggested filename returned by the HTTP server for this downloaded
        // file. The INetProvider implementor should set this when available.
        //
        virtual std::string SuggestedFileName() const = 0;
        virtual void SetSuggestedFileName(const std::string & fileName) = 0;
    };
}

#endif //ENABLE_NET_PROVIDER

#endif //__I_NET_PROVIDER_H__
