//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#ifndef __CRL_REVOCATION_LIST_UPDATER_H__
#define __CRL_REVOCATION_LIST_UPDATER_H__

#include <memory>
#include <mutex>
#include <condition_variable>
#include "ICertificate.h"
#include "SimpleMemoryWritableStream.h"
#include "public/INetProvider.h"
#include "NonCopyable.h"

namespace lcp
{
    class ThreadTimer;
    class CrlDownloader;
    class IDownloadRequest;

    class CrlUpdater : public INetProviderCallback, public NonCopyable
    {
    public:
        CrlUpdater(
            INetProvider * netProvider,
            ICertificateRevocationList * revocationList,
            ThreadTimer * threadTimer,
            const std::string & defaultCrlUrl
            );

        void Update();
        void Cancel();

        void UpdateCrlUrls(ICrlDistributionPoints * distributionPoints);
        bool ContainsUrl(const std::string & url);
        bool ContainsAnyUrl() const;

        // INetProviderCallback
        virtual void OnRequestStarted(INetRequest * request);
        virtual void OnRequestProgressed(INetRequest * request, float progress);
        virtual void OnRequestCanceled(INetRequest * request);
        virtual void OnRequestEnded(INetRequest * request, Status result);

    public:
        static const int TenMinutesPeriod;

    private:
        void Download(const std::string & url);
        void ResetNextUpdate();

    private:
        StringsList m_crlUrls;
        INetProvider * m_netProvider;
        ICertificateRevocationList * m_revocationList;
        ThreadTimer * m_threadTimer;

        std::unique_ptr<SimpleMemoryWritableStream> m_crlStream;
        std::unique_ptr<IDownloadRequest> m_downloadRequest;
        Status m_currentRequestStatus;

        bool m_requestRunning;
        mutable std::mutex m_downloadSync;
        std::condition_variable m_conditionDownload;
    };
}

#endif //__CRL_REVOCATION_LIST_UPDATER_H__
