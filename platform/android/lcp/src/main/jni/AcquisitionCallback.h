//
// Created by clebeaupin on 15/03/16.
//

#ifndef LCP_ANDROID_ACQUISITION_CALLBACK_H
#define LCP_ANDROID_ACQUISITION_CALLBACK_H

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

#endif //LCP_ANDROID_ACQUISITION_CALLBACK_H