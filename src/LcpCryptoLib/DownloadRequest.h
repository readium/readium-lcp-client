#ifndef __DOWNLOAD_REQUEST_H__
#define __DOWNLOAD_REQUEST_H__

#include <string>
#include "Public/INetProvider.h"
#include "Public/IFileSystemProvider.h"

namespace lcp
{
    class DownloadRequest : public lcp::IDownloadRequest
    {
    public:
        explicit DownloadRequest(const std::string & url, IFile * file)
            : m_url(url)
            , m_file(file)
            , m_canceled(false)
        {
        }
        virtual lcp::IFile * DestinationFile() const
        {
            return m_file;
        }
        virtual std::string SuggestedPath() const
        {
            return m_url;
        }
        virtual std::string Url() const
        {
            return m_url;
        }
        virtual bool Cancelled() const
        {
            return m_canceled;
        }
        virtual void SetCancelled(bool value)
        {
            m_canceled = value;
        }

    private:
        bool m_canceled;
        std::string m_url;
        IFile * m_file;
    };
}

#endif //__DOWNLOAD_REQUEST_H__
