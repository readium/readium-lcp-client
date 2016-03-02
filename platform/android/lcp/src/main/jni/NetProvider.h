//
// Created by clebeaupin on 29/01/16.
//

#ifndef LCP_ANDROID_NET_PROVIDER_H
#define LCP_ANDROID_NET_PROVIDER_H

#include <public/INetProvider.h>
#include <string>
#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

namespace lcp {

    class NetProvider: public INetProvider {
    public:
        void StartDownloadRequest(
                IDownloadRequest * request,
                INetProviderCallback * callback
        );
    };
}

#ifdef __cplusplus
}
#endif


#endif //LCP_ANDROID_NET_PROVIDER_H
