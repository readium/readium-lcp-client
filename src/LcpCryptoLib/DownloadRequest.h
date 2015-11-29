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
        explicit DownloadRequest(const std::string & url, IWritableFile * file)
            : m_url(url)
            , m_file(file)
            , m_canceled(false)
        {
            size_t pos = m_url.find_last_of('/');
            if (pos == std::string::npos || pos + 1 == m_url.size())
            {
                throw std::runtime_error("Wrong url format: " + m_url);
            }
            m_suggestedFileName = m_url.substr(pos + 1);
        }
        virtual lcp::IWritableFile * DestinationFile() const
        {
            return m_file;
        }
        virtual std::string SuggestedFileName() const
        {
            return m_suggestedFileName;
        }
        virtual std::string Url() const
        {
            return m_url;
        }
        virtual bool Canceled() const
        {
            return m_canceled;
        }
        virtual void SetCanceled(bool value)
        {
            m_canceled = value;
        }

    private:
        bool m_canceled;
        std::string m_url;
        std::string m_suggestedFileName;
        IWritableFile * m_file;
    };
}

#endif //__DOWNLOAD_REQUEST_H__
