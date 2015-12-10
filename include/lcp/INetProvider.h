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
        virtual void StartDownloadRequest(
            IDownloadRequest * request,
            INetProviderCallback * callback
            ) = 0;
        virtual void StartDownloadRequestAsync(
            IDownloadRequest * request,
            INetProviderCallback * callback
            ) = 0;
        virtual ~INetProvider() {}
    };
}

#endif //__I_NET_PROVIDER_H__
