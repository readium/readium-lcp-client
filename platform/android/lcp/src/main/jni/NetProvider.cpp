//
// Created by clebeaupin on 29/01/16.
//

#if !DISABLE_NET_PROVIDER

#include "NetProvider.h"
#include "Util.h"

namespace lcp {
    NetProvider::NetProvider(jobject jNetProvider) {
        JNIEnv * env = getJNIEnv();
        this->jNetProvider = env->NewGlobalRef(jNetProvider);
        jclass jNetProviderClass = env->GetObjectClass(this->jNetProvider);
        this->jDownloadMethodId = env->GetMethodID(jNetProviderClass, "download", "(Ljava/lang/String;Ljava/lang/String;JJ)V");
        this->jCancelMethodId = env->GetMethodID(jNetProviderClass, "cancel", "(J)V");
    }

    NetProvider::~NetProvider(){
        JNIEnv * env = getJNIEnv();
        env->DeleteGlobalRef(this->jNetProvider);
    }

    void NetProvider::StartDownloadRequest(
            IDownloadRequest * request,
            INetProviderCallback * callback
    ) {
        JNIEnv * env = getJNIEnv();
        jstring jUrl = env->NewStringUTF(request->Url().c_str());
        jstring jDstPath = env->NewStringUTF(request->DestinationPath().c_str());
        env->CallVoidMethod(this->jNetProvider, this->jDownloadMethodId, jUrl, jDstPath,
                                   (jlong) request, (jlong) callback);
    }

    void NetProvider::CancelDownloadRequest(
            IDownloadRequest * request
    ) {
        JNIEnv * env = getJNIEnv();
        env->CallVoidMethod(this->jNetProvider, this->jCancelMethodId, (jlong) request);
    }
}

JNIEXPORT void JNICALL Java_org_readium_sdk_lcp_NetProviderCallback_nativeOnRequestStarted(
        JNIEnv *env, jobject obj, jlong callbackPtr, jlong requestPtr) {
    lcp::INetProviderCallback * callback = (lcp::INetProviderCallback *) callbackPtr;
    lcp::IDownloadRequest * request = (lcp::IDownloadRequest *) requestPtr;
    callback->OnRequestStarted(request);
}

JNIEXPORT void JNICALL Java_org_readium_sdk_lcp_NetProviderCallback_nativeOnRequestEnded(
        JNIEnv *env, jobject obj, jlong callbackPtr, jlong requestPtr) {
    lcp::INetProviderCallback * callback = (lcp::INetProviderCallback *) callbackPtr;
    lcp::IDownloadRequest * request = (lcp::IDownloadRequest *) requestPtr;
    lcp::Status status(lcp::StatusCode::ErrorCommonSuccess);
    callback->OnRequestEnded(request, status);
}

JNIEXPORT void JNICALL Java_org_readium_sdk_lcp_NetProviderCallback_nativeOnRequestCanceled(
        JNIEnv *env, jobject obj, jlong callbackPtr, jlong requestPtr) {
    lcp::INetProviderCallback * callback = (lcp::INetProviderCallback *) callbackPtr;
    lcp::IDownloadRequest * request = (lcp::IDownloadRequest *) requestPtr;
    callback->OnRequestCanceled(request);
}

JNIEXPORT void JNICALL Java_org_readium_sdk_lcp_NetProviderCallback_nativeOnRequestProgressed(
        JNIEnv *env, jobject obj, jlong callbackPtr, jlong requestPtr, jfloat progress) {
    lcp::INetProviderCallback * callback = (lcp::INetProviderCallback *) callbackPtr;
    lcp::IDownloadRequest * request = (lcp::IDownloadRequest *) requestPtr;
    callback->OnRequestProgressed(request, progress);
}

#endif //!DISABLE_NET_PROVIDER