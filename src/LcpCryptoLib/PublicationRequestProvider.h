#pragma once

#include <memory>
#include "IDownloadRequestCreator.h"
#include "Public/ILinks.h"
#include "Public/INetProvider.h"

namespace lcp
{
    class IFile;
    class ILicense;
    class ICryptoProvider;
    class IFileSystemProvider;
    class IAcquisitionCallback;

    class PublicationRequestProvider : public IDownloadRequestCreator, public INetProviderCallback
    {
    public:
        explicit PublicationRequestProvider(ILicense * license);

        // IDownloadRequestCreator
        virtual Status CreateRequest(
            const std::string & path,
            IAcquisitionCallback * callback,
            IDownloadRequest ** request
            );

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
        IFileSystemProvider * m_fileSystemProvider;
        ICryptoProvider * m_cryptoProvider;

        Link m_publicationLink;
        IAcquisitionCallback * m_callback;
        std::unique_ptr<IFile> m_file;
        ILicense * m_license;
        Link m_publicationLink;
    };
}
