//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#ifndef __TEST_NET_PROVIDER_H__
#define __TEST_NET_PROVIDER_H__

#include <memory>
#include <string>
#include <sstream>
#include <thread>
#include <curl/curl.h>
#include "public/lcp.h"

/*#include "ICertificate.h"
#include <cryptopp/base64.h>
using namespace CryptoPP;

std::string testCrl = "MIIDFDCCAfwCAQEwDQYJKoZIhvcNAQEFBQAwXzEjMCEGA1UEChMaU2FtcGxlIFNp"
"Z25lciBPcmdhbml6YXRpb24xGzAZBgNVBAsTElNhbXBsZSBTaWduZXIgVW5pdDEb"
"MBkGA1UEAxMSU2FtcGxlIFNpZ25lciBDZXJ0Fw0xMzAyMTgxMDMyMDBaFw0xMzAy"
"MTgxMDQyMDBaMIIBNjA8AgMUeUcXDTEzMDIxODEwMjIxMlowJjAKBgNVHRUEAwoB"
"AzAYBgNVHRgEERgPMjAxMzAyMTgxMDIyMDBaMDwCAxR5SBcNMTMwMjE4MTAyMjIy"
"WjAmMAoGA1UdFQQDCgEGMBgGA1UdGAQRGA8yMDEzMDIxODEwMjIwMFowPAIDFHlJ"
"Fw0xMzAyMTgxMDIyMzJaMCYwCgYDVR0VBAMKAQQwGAYDVR0YBBEYDzIwMTMwMjE4"
"MTAyMjAwWjA8AgMUeUoXDTEzMDIxODEwMjI0MlowJjAKBgNVHRUEAwoBATAYBgNV"
"HRgEERgPMjAxMzAyMTgxMDIyMDBaMDwCAxR5SxcNMTMwMjE4MTAyMjUxWjAmMAoG"
"A1UdFQQDCgEFMBgGA1UdGAQRGA8yMDEzMDIxODEwMjIwMFqgLzAtMB8GA1UdIwQY"
"MBaAFL4SAcyq6hGA2i6tsurHtfuf+a00MAoGA1UdFAQDAgEDMA0GCSqGSIb3DQEB"
"BQUAA4IBAQBCIb6B8cN5dmZbziETimiotDy+FsOvS93LeDWSkNjXTG/+bGgnrm3a"
"QpgB7heT8L2o7s2QtjX2DaTOSYL3nZ/Ibn/R8S0g+EbNQxdk5/la6CERxiRp+E2T"
"UG8LDb14YVMhRGKvCguSIyUG0MwGW6waqVtd6K71u7vhIU/Tidf6ZSdsTMhpPPFu"
"PUid4j29U3q10SGFF6cCt1DzjvUcCwHGhHA02Men70EgZFADPLWmLg0HglKUh1iZ"
"WcBGtev/8VsUijyjsM072C6Ut5TwNyrrthb952+eKlmxLNgT0o5hVYxjXhtwLQsL"
"7QZhrypAM1DLYqQjkiDI7hlvt7QuDGTJ";*/

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
    void Worker(
        lcp::IDownloadRequest * request,
        lcp::INetProviderCallback * callback
        )
    {
        try
        {
            /* imitate CRL downloading
            lcp::Buffer rawDecoded;
            {
                Base64Decoder decoder;
                decoder.Put((byte *)testCrl.data(), testCrl.size());
                decoder.MessageEnd();

                lword size = decoder.MaxRetrievable();
                if (size > 0 && size <= SIZE_MAX)
                {
                    rawDecoded.resize(static_cast<size_t>(size));
                    decoder.Get((byte *)rawDecoded.data(), rawDecoded.size());
                }
            }

            request->DestinationStream()->SetWritePosition(0);
            request->DestinationStream()->Write(&rawDecoded.at(0), rawDecoded.size());
            callback->OnRequestEnded(request, lcp::Status(lcp::StCodeCover::ErrorCommonSuccess));
            return;*/

            CURL * curl;
            CURLcode res;
            curl = curl_easy_init();
            if (!curl)
            {
                callback->OnRequestEnded(request, lcp::Status(lcp::StatusCode::ErrorNetworkingRequestFailed, "Can not init library"));
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
                callback->OnRequestEnded(request, lcp::Status(lcp::StatusCode::ErrorCommonSuccess));
            }
            else
            {
                std::stringstream strm;
                strm << "Curl Error status: " << res;
                callback->OnRequestEnded(request, lcp::Status(lcp::StatusCode::ErrorNetworkingRequestFailed, strm.str().c_str()));
            }

            curl_easy_cleanup(curl);
        }
        catch (const std::exception & ex)
        {
            callback->OnRequestEnded(request, lcp::Status(lcp::StatusCode::ErrorCommonFail, ex.what()));
        }
    }

    virtual void StartDownloadRequest(
        lcp::IDownloadRequest * request,
        lcp::INetProviderCallback * callback
        )
    {
        std::thread downloadThread(&TestNetProvider::Worker, this, request, callback);
        downloadThread.detach();
    }
};

#endif //__TEST_NET_PROVIDER_H__