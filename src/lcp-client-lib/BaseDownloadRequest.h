// Copyright (c) 2016 Mantano
// Licensed to the Readium Foundation under one or more contributor license agreements.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation and/or
//    other materials provided with the distribution.
// 3. Neither the name of the organization nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#ifndef __BASE_DOWNLOAD_REQUEST_H__
#define __BASE_DOWNLOAD_REQUEST_H__

#if ENABLE_NET_PROVIDER

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

#endif //ENABLE_NET_PROVIDER

#endif //__BASE_DOWNLOAD_REQUEST_H__
