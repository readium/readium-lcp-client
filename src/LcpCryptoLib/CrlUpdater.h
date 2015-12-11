//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//  Licensed under Gnu Affero General Public License Version 3 (provided, notwithstanding this notice,
//  Readium Foundation reserves the right to license this material under a different separate license,
//  and if you have done so, the terms of that separate license control and the following references
//  to GPL do not apply).
//
//  This program is free software: you can redistribute it and/or modify it under the terms of the GNU
//  Affero General Public License as published by the Free Software Foundation, either version 3 of
//  the License, or (at your option) any later version. You should have received a copy of the GNU
//  Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef __CRL_REVOCATION_LIST_UPDATER_H__
#define __CRL_REVOCATION_LIST_UPDATER_H__

#include <memory>
#include <mutex>
#include <condition_variable>
#include "ICertificate.h"
#include "SimpleMemoryWritableStream.h"
#include "Public/INetProvider.h"

namespace lcp
{
    class ThreadTimer;
    class CrlDownloader;
    class IDownloadRequest;

    class CrlUpdater : public INetProviderCallback
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
