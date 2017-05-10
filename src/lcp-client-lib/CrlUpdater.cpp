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


#if !DISABLE_CRL

#include <algorithm>
#include <iterator>
#include <thread>
#include "CrlUpdater.h"

#if !DISABLE_NET_PROVIDER
#if !DISABLE_CRL_DOWNLOAD_IN_MEMORY
#include "SimpleMemoryWritableStream.h"
#include "DownloadInMemoryRequest.h"
#else // !DISABLE_CRL_DOWNLOAD_IN_MEMORY
#include "DownloadInFileRequest.h"
#endif // !DISABLE_CRL_DOWNLOAD_IN_MEMORY

#endif //!DISABLE_NET_PROVIDER

#include "DateTime.h"
#include "ThreadTimer.h"

namespace lcp
{
    const int CrlUpdater::TenMinutesPeriod = 1000 * 60 * 10;
    
    CrlUpdater::CrlUpdater(
#if !DISABLE_NET_PROVIDER
        INetProvider * netProvider,
#endif //!DISABLE_NET_PROVIDER

            IFileSystemProvider * fileSystemProvider,

        ICertificateRevocationList * revocationList,
#if !DISABLE_CRL_BACKGROUND_POLL
        ThreadTimer * threadTimer,
#endif //!DISABLE_CRL_BACKGROUND_POLL
        const std::string & defaultCrlUrl
        )
        : m_requestRunning(false)
#if !DISABLE_NET_PROVIDER
        , m_netProvider(netProvider)
#endif //!DISABLE_NET_PROVIDER

            , m_fileSystemProvider(fileSystemProvider)

        , m_revocationList(revocationList)
#if !DISABLE_CRL_BACKGROUND_POLL
          , m_threadTimer(threadTimer)
#endif //!DISABLE_CRL_BACKGROUND_POLL
        , m_currentRequestStatus(Status(StatusCode::ErrorCommonSuccess))
    {
        if (!defaultCrlUrl.empty())
        {
            m_crlUrls.push_back(defaultCrlUrl);
        }
    }

    void CrlUpdater::UpdateCrlUrls(ICrlDistributionPoints * distributionPoints)
    {
        std::unique_lock<std::mutex> locker(m_downloadSync);

        if (distributionPoints != nullptr && distributionPoints->HasCrlDistributionPoints())
        {
            const StringsList & newUrls = distributionPoints->CrlDistributionPointUrls();
            std::copy_if(
                newUrls.begin(), newUrls.end(),
                std::back_inserter(m_crlUrls),
                [&](const std::string & val) { return (std::find(m_crlUrls.begin(), m_crlUrls.end(), val) == m_crlUrls.end()); }
            );
        }
    }

    bool CrlUpdater::ContainsUrl(const std::string & url)
    {
        std::unique_lock<std::mutex> locker(m_downloadSync);
        return (std::find(m_crlUrls.begin(), m_crlUrls.end(), url) != m_crlUrls.end());
    }

    bool CrlUpdater::ContainsAnyUrl() const
    {
        std::unique_lock<std::mutex> locker(m_downloadSync);
        return !m_crlUrls.empty();
    }

    void CrlUpdater::Update()
    {
        std::unique_lock<std::mutex> locker(m_downloadSync);
        
        // If the list will be changed, it won't affect current update
        StringsList curUrls = m_crlUrls;

#if !DISABLE_NET_PROVIDER
        m_currentRequestStatus = Status(StatusCode::ErrorNetworkingRequestFailed, "ErrorNetworkingRequestFailed");
#else
        m_currentRequestStatus = Status(StatusCode::ErrorCommonSuccess);
#endif //!DISABLE_NET_PROVIDER

        for (auto const & url : curUrls)
        {
            this->Download(url);
            m_conditionDownload.wait(locker, [&]() { return !m_requestRunning; });

            if (Status::IsSuccess(m_currentRequestStatus)
#if !DISABLE_NET_PROVIDER
                || m_downloadRequest->Canceled()
#endif //!DISABLE_NET_PROVIDER
                    )
            {
                break;
            }
        }
    }

    void CrlUpdater::Cancel()
    {
        std::unique_lock<std::mutex> locker(m_downloadSync);
#if !DISABLE_NET_PROVIDER
        if (m_downloadRequest.get() != nullptr)
        {
            m_downloadRequest->SetCanceled(true);
        }
#endif //!DISABLE_NET_PROVIDER
    }

    void CrlUpdater::Download(const std::string & url)
    {
#if !DISABLE_NET_PROVIDER
        m_requestRunning = true;

#if !DISABLE_CRL_DOWNLOAD_IN_MEMORY
        m_crlStream.reset(new SimpleMemoryWritableStream()); // not actually used! (NetProvider Java operates on File)
        m_downloadRequest.reset(new DownloadInMemoryRequest(url, m_crlStream.get()));
#else // !DISABLE_CRL_DOWNLOAD_IN_MEMORY
        m_crlFile.reset(m_fileSystemProvider->GetFile(PATH_TO_DOWNLOAD));
        m_downloadRequest.reset(new DownloadInFileRequest(url, m_crlFile.get()));
#endif // !DISABLE_CRL_DOWNLOAD_IN_MEMORY

        m_netProvider->StartDownloadRequest(m_downloadRequest.get(), this);
#else
        m_currentRequestStatus = Status(StatusCode::ErrorCommonSuccess);
        m_requestRunning = false;
        m_conditionDownload.notify_one();
#endif //!DISABLE_NET_PROVIDER
    }

#if !DISABLE_NET_PROVIDER
    void CrlUpdater::OnRequestStarted(INetRequest * request)
    {
        bool breakpoint = true;
    }

    void CrlUpdater::OnRequestProgressed(INetRequest * request, float progress)
    {
        bool breakpoint = true;
    }

    void CrlUpdater::OnRequestCanceled(INetRequest * request)
    {
        std::unique_lock<std::mutex> locker(m_downloadSync);
        m_requestRunning = false;
        m_conditionDownload.notify_one();
    }

    void CrlUpdater::OnRequestEnded(INetRequest * request, Status result)
    {
        try
        {
            std::unique_lock<std::mutex> locker(m_downloadSync);
            if (Status::IsSuccess(result))
            {
#if !DISABLE_CRL_DOWNLOAD_IN_MEMORY
                lcp::BaseDownloadRequest* request_ = dynamic_cast<lcp::BaseDownloadRequest*>(request);
                if (request_) {
                    std::string path = request_->SuggestedFileName();
                    if (path.length()) {
                        IFile* file = m_fileSystemProvider->GetFile(path, IFileSystemProvider::ReadOnly);

                        size_t bufferSize = 1024 * 1024;
                        std::vector<unsigned char> buffer(bufferSize);

                        size_t read = 0;
                        size_t sizeToRead = bufferSize;
                        size_t fileSize = static_cast<size_t>(file->Size());
                        while (read != fileSize)
                        {
                            sizeToRead = (fileSize - read > bufferSize) ? bufferSize : fileSize - read;
                            file->Read(buffer.data(), sizeToRead);
                            read += sizeToRead;
                        }

                        delete file;

                        m_revocationList->UpdateRevocationList(buffer); //std::vector<unsigned char>
                    }
                }
                // m_revocationList->UpdateRevocationList(m_crlStream->Buffer()); //std::vector<unsigned char>

#else // !DISABLE_CRL_DOWNLOAD_IN_MEMORY
                // IFile == IReadableStream
                size_t bufferSize = 1024 * 1024;
                std::vector<unsigned char> buffer(bufferSize);

                size_t read = 0;
                size_t sizeToRead = bufferSize;
                size_t fileSize = static_cast<size_t>(m_crlFile->Size());
                while (read != fileSize)
                {
                    sizeToRead = (fileSize - read > bufferSize) ? bufferSize : fileSize - read;
                    m_crlFile->Read(buffer.data(), sizeToRead);
                    read += sizeToRead;
                }

                m_revocationList->UpdateRevocationList(buffer); //std::vector<unsigned char>
#endif // !DISABLE_CRL_DOWNLOAD_IN_MEMORY

                this->ResetNextUpdate();
            }
            m_currentRequestStatus = result;
            m_requestRunning = false;
            m_conditionDownload.notify_one();
        }
        catch (const std::exception & ex)
        {
            m_currentRequestStatus = Status(StatusCode::ErrorNetworkingRequestFailed, "ErrorNetworkingRequestFailed: " + std::string(ex.what()));
            m_requestRunning = false;
            m_conditionDownload.notify_one();
        }
    }
#endif //!DISABLE_NET_PROVIDER

    void CrlUpdater::ResetNextUpdate()
    {
#if !DISABLE_CRL_BACKGROUND_POLL
        if (m_revocationList->HasNextUpdateDate())
        {
            DateTime nextUpdate(m_revocationList->NextUpdateDate());
            if (nextUpdate > DateTime::Now())
            {
                m_threadTimer->SetUsage(ThreadTimer::TimePointUsage);
                m_threadTimer->SetTimePoint(ThreadTimer::ClockType::from_time_t(nextUpdate.ToTime()));
            }
            else
            {
                m_threadTimer->SetUsage(ThreadTimer::DurationUsage);
                m_threadTimer->SetDuration(ThreadTimer::DurationType(TenMinutesPeriod));
            }
        }
        else
        {
            m_threadTimer->SetUsage(ThreadTimer::DurationUsage);
            m_threadTimer->SetDuration(ThreadTimer::DurationType(TenMinutesPeriod));
        }
//
//// TODO: remove this, for testing async update only!
//        m_threadTimer->SetUsage(ThreadTimer::DurationUsage);
//        m_threadTimer->SetDuration(ThreadTimer::DurationType(2000)); //TenMinutesPeriod == 1000 * 60 * 10

        m_threadTimer->SetAutoReset(true);
#endif //!DISABLE_CRL_BACKGROUND_POLL
    }
}

#endif //!DISABLE_CRL