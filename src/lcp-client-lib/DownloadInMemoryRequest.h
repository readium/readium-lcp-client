//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#ifndef __DOWNLOAD_IN_MEMORY_REQUEST_H__
#define __DOWNLOAD_IN_MEMORY_REQUEST_H__

#if ENABLE_NET_PROVIDER

#include "BaseDownloadRequest.h"

namespace lcp
{
    class DownloadInMemoryRequest : public BaseDownloadRequest
    {
    public:
        DownloadInMemoryRequest(const std::string & url, IWritableStream * stream)
            : BaseDownloadRequest(url)
            , m_stream(stream)
        {
        }

        virtual IWritableStream * DestinationStream() const
        {
            return m_stream;
        }

        virtual bool HasDestinationPath() const
        {
            return false;
        }

        virtual std::string DestinationPath() const
        {
            return std::string();
        }

    private:
        IWritableStream * m_stream;
    };
}

#endif //ENABLE_NET_PROVIDER

#endif //__DOWNLOAD_IN_MEMORY_REQUEST_H__
