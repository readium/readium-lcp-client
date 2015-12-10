#ifndef __DOWNLOAD_IN_FILE_REQUEST_H__
#define __DOWNLOAD_IN_FILE_REQUEST_H__

#include <string>
#include "BaseDownloadRequest.h"
#include "Public/IFileSystemProvider.h"

namespace lcp
{
    class DownloadInFileRequest : public BaseDownloadRequest
    {
    public:
        DownloadInFileRequest(const std::string & url, IFile * file)
            : BaseDownloadRequest(url)
            , m_file(file)
        {
        }

        virtual IWritableStream * DestinationStream() const
        {
            return m_file;
        }

        virtual bool HasDesinationPath() const
        {
            return true;
        }

        virtual std::string DestinationPath() const
        {
            return m_file->GetPath();
        }

    private:
        IFile * m_file;
    };
}

#endif //__DOWNLOAD_IN_FILE_REQUEST_H__
