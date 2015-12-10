//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#ifndef __BASE_DOWNLOAD_REQUEST_H__
#define __BASE_DOWNLOAD_REQUEST_H__

#include "Public/INetProvider.h"

namespace lcp
{
    class BaseDownloadRequest : public IDownloadRequest
    {
    public:
        BaseDownloadRequest(const std::string & url)
            : m_url(url)
            , m_canceled(false)
        {
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

        virtual void SetSuggestedFileName(const std::string & fileName)
        {
            m_suggestedFileName = fileName;
        }

        virtual std::string SuggestedFileName() const
        {
            return m_suggestedFileName;
        }

    protected:
        bool m_canceled;
        std::string m_url;
        std::string m_suggestedFileName;
    };
}

#endif //__BASE_DOWNLOAD_REQUEST_H__
