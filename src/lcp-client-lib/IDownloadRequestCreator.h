#ifndef __I_DOWNLOAD_REQUEST_CREATOR_H__
#define __I_DOWNLOAD_REQUEST_CREATOR_H__

#include "Public/LcpStatus.h"

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

#endif //__I_DOWNLOAD_REQUEST_CREATOR_H__
