//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#ifndef __I_DOWNLOAD_REQUEST_CREATOR_H__
#define __I_DOWNLOAD_REQUEST_CREATOR_H__

#if ENABLE_NET_PROVIDER

#include "public/LcpStatus.h"

namespace lcp
{
    class IDownloadRequest;
    class IAcquisitionCallback;

    class IDownloadRequestCreator
    {
    public:
        virtual Status CreateDownloadRequest(
            const std::string & path,
            IAcquisitionCallback * callback,
            IDownloadRequest ** request
            ) = 0;
        virtual ~IDownloadRequestCreator() {}
    };
}

#endif //ENABLE_NET_PROVIDER

#endif //__I_DOWNLOAD_REQUEST_CREATOR_H__
