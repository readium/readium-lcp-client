// Copyright (c) 2016 Readium
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