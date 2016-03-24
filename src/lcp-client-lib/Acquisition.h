//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#ifndef __LCP_ACQUISITION_H__
#define __LCP_ACQUISITION_H__

#include <string>
#include <mutex>
#include <memory>
#include "public/IAcquistion.h"
#include "public/IAcquistionCallback.h"
#include "public/INetProvider.h"
#include "public/IFileSystemProvider.h"
#include "public/ILinks.h"
#include "NonCopyable.h"

namespace lcp
{
    class ILicense;
    class ICryptoProvider;

    class Acquisition : public IAcquisition, public INetProviderCallback, public NonCopyable
    {
    public:
        Acquisition(
            ILicense * license,
            IFileSystemProvider * fileSystemProvider,
            INetProvider * netProvider,
            ICryptoProvider * cryptoProvider,
            const std::string & publicationPath
            );

        virtual Status Start(IAcquisitionCallback * callback);
        virtual Status Cancel();
        virtual std::string PublicationPath() const;
        virtual std::string SuggestedFileName() const;

    public:
        // INetProviderCallback
        virtual void OnRequestStarted(INetRequest * request);
        virtual void OnRequestProgressed(INetRequest * request, float progress);
        virtual void OnRequestCanceled(INetRequest * request);
        virtual void OnRequestEnded(INetRequest * request, Status result);

    public:
        static const char * PublicationType;

    private:
        Status CheckPublicationHash(const Link & link);

    private:
        ILicense * m_license;
        IFileSystemProvider * m_fileSystemProvider;
        INetProvider * m_netProvider;
        ICryptoProvider * m_cryptoProvider;
        std::string m_publicationPath;

        mutable std::mutex m_sync;
        Link m_publicationLink;
        IAcquisitionCallback * m_callback;
        std::unique_ptr<IFile> m_file;
        std::unique_ptr<IDownloadRequest> m_request;
    };
}

#endif //__LCP_ACQUISITION_H__
