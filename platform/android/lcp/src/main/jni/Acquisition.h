//
// Created by clebeaupin on 15/03/16.
//

#ifndef LCP_ANDROID_ACQUISITION_H
#define LCP_ANDROID_ACQUISITION_H

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL Java_org_readium_sdk_lcp_Acquisition_nativeStart(
        JNIEnv *env, jobject obj, jobject jAcquisitionCallback, jlong acquisitionPtr);

JNIEXPORT void JNICALL Java_org_readium_sdk_lcp_Acquisition_nativeCancel(
        JNIEnv *env, jobject obj, jlong acquisitionPtr);

#ifdef __cplusplus
}
#endif

#endif //LCP_ANDROID_ACQUISITION_H
