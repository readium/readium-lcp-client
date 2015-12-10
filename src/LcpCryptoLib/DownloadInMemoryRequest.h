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

        virtual bool HasDesinationPath() const
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
