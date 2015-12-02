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

        virtual lcp::IWritableStream * DestinationStream() const
        {
            return m_file;
        }

        virtual std::string DestinationPath() const
        {
            return m_file->GetPath();
        }

        virtual void SetSuggestedFileName(const std::string & fileName)
        {
            m_suggestedFileName = fileName;
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
        IFile * m_file;
    };
}

#endif //__DOWNLOAD_REQUEST_H__
