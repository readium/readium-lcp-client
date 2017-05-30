// Licensed to the Readium Foundation under one or more contributor license agreements.
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


#ifndef LCP_ANDROID_ACQUISITION_CALLBACK_H
#define LCP_ANDROID_ACQUISITION_CALLBACK_H

#if ENABLE_NET_PROVIDER_ACQUISITION

#include <jni.h>
#include <public/IAcquistionCallback.h>

#ifdef __cplusplus
extern "C" {
#endif

namespace lcp {
    class AcquisitionCallback: public IAcquisitionCallback {
    private:
        jobject jAcquisitionCallback;
        jmethodID jOnAcquisitionStartedMethodId;
        jmethodID jOnAcquisitionEndedMethodId;
        jmethodID jOnAcquisitionProgressedMethodId;
        jmethodID jOnAcquisitionCanceledMethodId;
    public:
        AcquisitionCallback(jobject jAcquisitionCallback);
        ~AcquisitionCallback();
        void OnAcquisitionStarted(IAcquisition * acquisition);
        void OnAcquisitionProgressed(IAcquisition * acquisition, float progress);
        void OnAcquisitionCanceled(IAcquisition * acquisition);
        void OnAcquisitionEnded(IAcquisition * acquisition, Status result);
    };
}

#ifdef __cplusplus
}
#endif

#endif //ENABLE_NET_PROVIDER_ACQUISITION

#endif //LCP_ANDROID_ACQUISITION_CALLBACK_H