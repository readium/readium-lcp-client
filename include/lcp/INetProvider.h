#ifndef __I_NET_PROVIDER_H__
#define __I_NET_PROVIDER_H__

namespace lcp
{
    class IFile;

    class INetRequest
    {
    public:
        virtual std::string Url() const = 0;
        virtual bool Canceled() const = 0;
        virtual void SetCanceled(bool value) = 0;
        virtual ~INetRequest() {}
    };

    class IDownloadRequest : public INetRequest
    {
    public:
        virtual IFile * DestinationFile() const = 0;
        virtual std::string SuggestedPath() const = 0;
    };

    class INetProviderCallback
    {
    public:
        virtual void OnRequestStarted(INetRequest * request) = 0;
        virtual void OnRequestProgressed(INetRequest * request, float progress) = 0;
        virtual void OnRequestCanceled(INetRequest * request) = 0;
        virtual void OnRequestEnded(INetRequest * request, Status result) = 0;
        virtual ~INetProviderCallback() {}
    };

    class INetProvider
    {
    public:
        virtual void StartDownloadRequest(
            IDownloadRequest * request,
            INetProviderCallback * callback
            ) = 0;
        virtual ~INetProvider() {}
    };
}

#endif //__I_NET_PROVIDER_H__
