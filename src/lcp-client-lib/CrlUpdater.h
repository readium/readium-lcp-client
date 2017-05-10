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


#ifndef __CRL_REVOCATION_LIST_UPDATER_H__
#define __CRL_REVOCATION_LIST_UPDATER_H__

#if !DISABLE_CRL

#include <memory>
#include <mutex>
#include <condition_variable>
#include "public/lcp.h"
#include "ICertificate.h"

#if !DISABLE_NET_PROVIDER
#include "public/INetProvider.h"
#endif //!DISABLE_NET_PROVIDER

#include "NonCopyable.h"

#if !DISABLE_CRL_DOWNLOAD_IN_MEMORY
#include "SimpleMemoryWritableStream.h"
#endif // !DISABLE_CRL_DOWNLOAD_IN_MEMORY

#include "IFileSystemProvider.h"

namespace lcp
{
    class ThreadTimer;
    class CrlDownloader;
#if !DISABLE_NET_PROVIDER
    class IDownloadRequest;
#endif //!DISABLE_NET_PROVIDER

    class CrlUpdater :
#if !DISABLE_NET_PROVIDER
            public INetProviderCallback,
#endif //!DISABLE_NET_PROVIDER
            public NonCopyable
    {
    public:
        CrlUpdater(
#if !DISABLE_NET_PROVIDER
            INetProvider * netProvider,
#endif //!DISABLE_NET_PROVIDER

                IFileSystemProvider * fileSystemProvider,

            ICertificateRevocationList * revocationList,
#if !DISABLE_CRL_BACKGROUND_POLL
            ThreadTimer * threadTimer,
#endif //!DISABLE_CRL_BACKGROUND_POLL
            const std::string & defaultCrlUrl
            );

        void Update();
        void Cancel();

        void UpdateCrlUrls(ICrlDistributionPoints * distributionPoints);
        bool ContainsUrl(const std::string & url);
        bool ContainsAnyUrl() const;

#if !DISABLE_NET_PROVIDER
        // INetProviderCallback
        virtual void OnRequestStarted(INetRequest * request);
        virtual void OnRequestProgressed(INetRequest * request, float progress);
        virtual void OnRequestCanceled(INetRequest * request);
        virtual void OnRequestEnded(INetRequest * request, Status result);
#endif //!DISABLE_NET_PROVIDER

    public:
        static const int TenMinutesPeriod;

    private:
        void Download(const std::string & url);
        void ResetNextUpdate();

    private:
        StringsList m_crlUrls;
#if !DISABLE_NET_PROVIDER
        INetProvider * m_netProvider;
#endif //!DISABLE_NET_PROVIDER
        ICertificateRevocationList * m_revocationList;
#if !DISABLE_CRL_BACKGROUND_POLL
        ThreadTimer * m_threadTimer;
#endif //!DISABLE_CRL_BACKGROUND_POLL

#if !DISABLE_NET_PROVIDER

        IFileSystemProvider * m_fileSystemProvider;

#if !DISABLE_CRL_DOWNLOAD_IN_MEMORY
        std::unique_ptr<SimpleMemoryWritableStream> m_crlStream;
#else // !DISABLE_CRL_DOWNLOAD_IN_MEMORY
        std::unique_ptr<IFile> m_crlFile;
#endif // !DISABLE_CRL_DOWNLOAD_IN_MEMORY

        std::unique_ptr<IDownloadRequest> m_downloadRequest;
#endif //!DISABLE_NET_PROVIDER

        Status m_currentRequestStatus;

        bool m_requestRunning;
        mutable std::mutex m_downloadSync;
        std::condition_variable m_conditionDownload;
    };
}

#endif //!DISABLE_CRL

#endif //__CRL_REVOCATION_LIST_UPDATER_H__
