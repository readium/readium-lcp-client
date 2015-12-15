//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#ifndef __DOWNLOAD_IN_MEMORY_REQUEST_H__
#define __DOWNLOAD_IN_MEMORY_REQUEST_H__

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

#endif //__DOWNLOAD_IN_MEMORY_REQUEST_H__
