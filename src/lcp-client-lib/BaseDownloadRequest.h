//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#ifndef __BASE_DOWNLOAD_REQUEST_H__
#define __BASE_DOWNLOAD_REQUEST_H__

#include <atomic>
#include <mutex>
#include "public/INetProvider.h"

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
            std::unique_lock<std::mutex> locker(m_suggestedNameSync);
            m_suggestedFileName = fileName;
        }

        virtual std::string SuggestedFileName() const
        {
            std::unique_lock<std::mutex> locker(m_suggestedNameSync);
            return m_suggestedFileName;
        }

    protected:
        std::atomic<bool> m_canceled;
        std::string m_url;
        std::string m_suggestedFileName;

    private:
        mutable std::mutex m_suggestedNameSync;
    };
}

#endif //__BASE_DOWNLOAD_REQUEST_H__
