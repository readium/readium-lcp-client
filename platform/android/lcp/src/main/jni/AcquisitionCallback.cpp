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

#include "AcquisitionCallback.h"
#include "Util.h"

namespace lcp {
    AcquisitionCallback::AcquisitionCallback(jobject jAcquisitionCallback) {
        JNIEnv * env = getJNIEnv();
        this->jAcquisitionCallback = env->NewGlobalRef(jAcquisitionCallback);
        jclass jAcquisitionCallbackClass = env->GetObjectClass(this->jAcquisitionCallback);
        this->jOnAcquisitionStartedMethodId = env->GetMethodID(
                jAcquisitionCallbackClass, "onAcquisitionStarted", "()V");
        this->jOnAcquisitionEndedMethodId = env->GetMethodID(
                jAcquisitionCallbackClass, "onAcquisitionEnded", "()V");
        this->jOnAcquisitionProgressedMethodId = env->GetMethodID(
                jAcquisitionCallbackClass, "onAcquisitionProgressed", "(F)V");
        this->jOnAcquisitionCanceledMethodId = env->GetMethodID(
                jAcquisitionCallbackClass, "onAcquisitionCanceled", "()V");
    }

    AcquisitionCallback::~AcquisitionCallback() {
        JNIEnv * env = getJNIEnv();
        env->DeleteGlobalRef(this->jAcquisitionCallback);
    }

    void AcquisitionCallback::OnAcquisitionStarted(IAcquisition * acquisition) {
        JNIEnv * env = getJNIEnv();
        env->CallVoidMethod(
                this->jAcquisitionCallback, jOnAcquisitionStartedMethodId);
    }

    void AcquisitionCallback::OnAcquisitionProgressed(IAcquisition * acquisition, float progress) {
        JNIEnv * env = getJNIEnv();
        env->CallVoidMethod(
                this->jAcquisitionCallback, jOnAcquisitionProgressedMethodId, (jfloat) progress);
    }

    void AcquisitionCallback::OnAcquisitionCanceled(IAcquisition * acquisition) {
        JNIEnv * env = getJNIEnv();
        env->CallVoidMethod(
                this->jAcquisitionCallback, jOnAcquisitionCanceledMethodId);
    }

    void AcquisitionCallback::OnAcquisitionEnded(IAcquisition * acquisition, Status result) {
        JNIEnv * env = getJNIEnv();
        env->CallVoidMethod(
                this->jAcquisitionCallback, jOnAcquisitionEndedMethodId);
    }
}