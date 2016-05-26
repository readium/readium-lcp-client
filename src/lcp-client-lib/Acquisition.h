// Copyright (c) 2016 Mantano, Readium.
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
