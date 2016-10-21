//
// Created by clebeaupin on 15/03/16.
//

#if ENABLE_NET_PROVIDER

#include <public/IAcquistion.h>
#include <public/IAcquistionCallback.h>
#include "Acquisition.h"
#include "AcquisitionCallback.h"


JNIEXPORT void JNICALL Java_org_readium_sdk_lcp_Acquisition_nativeStart(
        JNIEnv *env, jobject obj, jobject jAcquisitionCallback, jlong acquisitionPtr) {
    lcp::IAcquisition * acquisition = (lcp::IAcquisition *) acquisitionPtr;
    lcp::IAcquisitionCallback * acquisitionCallback;

    if (jAcquisitionCallback != nullptr) {
        acquisitionCallback = new lcp::AcquisitionCallback(jAcquisitionCallback);
    }

    acquisition->Start(acquisitionCallback);
}

JNIEXPORT void JNICALL Java_org_readium_sdk_lcp_Acquisition_nativeCancel(
        JNIEnv *env, jobject obj, jlong acquisitionPtr) {
    lcp::IAcquisition * acquisition = (lcp::IAcquisition *) acquisitionPtr;

    acquisition->Cancel();
}

#endif //ENABLE_NET_PROVIDER