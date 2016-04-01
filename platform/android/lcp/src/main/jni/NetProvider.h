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
    class NetRequest: public INetRequest {

    };

    class NetProvider: public INetProvider {
    private:
        jobject jNetProvider;
        jmethodID jDownloadMethodId;
        jmethodID jCancelMethodId;
    public:
        NetProvider(jobject jNetProvider);
        ~NetProvider();
        void StartDownloadRequest(
                IDownloadRequest * request,
                INetProviderCallback * callback
        );
        void CancelDownloadRequest(IDownloadRequest * request);
    };
}

JNIEXPORT void JNICALL Java_org_readium_sdk_lcp_NetProviderCallback_nativeOnRequestStarted(
        JNIEnv *env, jobject obj, jlong callbackPtr, jlong requestPtr);
JNIEXPORT void JNICALL Java_org_readium_sdk_lcp_NetProviderCallback_nativeOnRequestEnded(
        JNIEnv *env, jobject obj, jlong callbackPtr, jlong requestPtr);
JNIEXPORT void JNICALL Java_org_readium_sdk_lcp_NetProviderCallback_nativeOnRequestCanceled(
        JNIEnv *env, jobject obj, jlong callbackPtr, jlong requestPtr);
JNIEXPORT void JNICALL Java_org_readium_sdk_lcp_NetProviderCallback_nativeOnRequestProgressed(
        JNIEnv *env, jobject obj, jlong callbackPtr, jlong requestPtr, jfloat progress);


#ifdef __cplusplus
}
#endif


#endif //LCP_ANDROID_NET_PROVIDER_H
