//
// Created by clebeaupin on 15/03/16.
//

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