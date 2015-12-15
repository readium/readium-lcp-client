//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#ifndef __DOWNLOAD_IN_FILE_REQUEST_H__
#define __DOWNLOAD_IN_FILE_REQUEST_H__

#include <string>
#include "BaseDownloadRequest.h"
#include "public/IFileSystemProvider.h"

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

        virtual bool HasDestinationPath() const
        {
            return true;
        }

        virtual std::string DestinationPath() const
        {
            return m_file->Path();
        }

    private:
        IFile * m_file;
    };
}

#endif //__DOWNLOAD_IN_FILE_REQUEST_H__
