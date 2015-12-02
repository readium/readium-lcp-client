#ifndef __TEST_NET_PROVIDER_H__
#define __TEST_NET_PROVIDER_H__

#include <memory>
#include <string>
#include <sstream>
#include <curl/curl.h>
#include "Public/lcp.h"

size_t CallbackWriteFile(void *ptr, size_t size, size_t nmemb, lcp::IWritableStream * stream)
{
    try
    {
        size_t sizeToWrite = size * nmemb;
        stream->Write(static_cast<const unsigned char *>(ptr), sizeToWrite);
        return sizeToWrite;
    }
    catch (const std::exception & ex)
    {
        ex;
        return -1;
    }
    return 0;
}


struct NetworkInfo
{
    lcp::INetRequest * request;
    lcp::INetProviderCallback * callback;
};

int CallbackProgress(NetworkInfo * networkInfo, double t, double d, double ultotal, double ulnow)
{
    try
    {
        if (networkInfo->callback != nullptr)
        {
            float progress = 0;
            if (t != 0)
            {
                progress = static_cast<float>(d / t);
            }
            networkInfo->callback->OnRequestProgressed(networkInfo->request, progress);
        }
    }
    catch (const std::exception & ex)
    {
        ex;
        return -1;
    }
    return 0;
}

class TestNetProvider : public lcp::INetProvider
{
public:
    virtual void StartDownloadRequest(
        lcp::IDownloadRequest * request,
        lcp::INetProviderCallback * callback
        )
    {
        CURL * curl;
        CURLcode res;
        curl = curl_easy_init();
        if (!curl)
        {
            callback->OnRequestEnded(request, lcp::Status(lcp::StCodeCover::ErrorNetworkingRequestFailed, "Can not init library"));
            return;
        }

        curl_easy_setopt(curl, CURLOPT_URL, request->Url().c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CallbackWriteFile);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, request->DestinationStream());
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, CallbackProgress);

        std::unique_ptr<NetworkInfo> networkInfo(new NetworkInfo());
        networkInfo->request = request;
        networkInfo->callback = callback;
        curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, networkInfo.get());

        callback->OnRequestStarted(request);
        res = curl_easy_perform(curl);
        if (res == CURLE_OK)
        {
            callback->OnRequestEnded(request, lcp::Status(lcp::StCodeCover::ErrorCommonSuccess));
        }
        else
        {
            std::stringstream strm;
            strm << "Curl Error status: " << res;
            callback->OnRequestEnded(request, lcp::Status(lcp::StCodeCover::ErrorNetworkingRequestFailed, strm.str().c_str()));
        }

        curl_easy_cleanup(curl);
    }
};

#endif //__TEST_NET_PROVIDER_H__