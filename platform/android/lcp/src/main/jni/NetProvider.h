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
    private:
        jobject jNetProvider;
        jmethodID jDownloadMethodId;
    public:
        NetProvider(jobject jNetProvider);
        ~NetProvider();
        void StartDownloadRequest(
                IDownloadRequest * request,
                INetProviderCallback * callback
        );
    };
}

JNIEXPORT void JNICALL Java_org_readium_sdk_lcp_NetProviderCallback_nativeOnRequestStarted(
        JNIEnv *env, jobject obj, jlong callbackPtr, jlong requestPtr);
JNIEXPORT void JNICALL Java_org_readium_sdk_lcp_NetProviderCallback_nativeOnRequestEnded(
        JNIEnv *env, jobject obj, jlong callbackPtr, jlong requestPtr);
JNIEXPORT void JNICALL Java_org_readium_sdk_lcp_NetProviderCallback_nativeOnRequestProgressed(
        JNIEnv *env, jobject obj, jlong callbackPtr, jlong requestPtr, jfloat progress);


#ifdef __cplusplus
}
#endif


#endif //LCP_ANDROID_NET_PROVIDER_H
