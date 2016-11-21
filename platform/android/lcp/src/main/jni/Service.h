//
// Created by clebeaupin on 05/02/16.
//

#ifndef LCP_ANDROID_SERVICE_H
#define LCP_ANDROID_SERVICE_H

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

namespace lcp {
    class Service {

    };
}

JNIEXPORT jobject JNICALL Java_org_readium_sdk_lcp_Service_nativeOpenLicense(
        JNIEnv *env, jobject obj, jlong servicePtr, jstring jLicenseJson);

JNIEXPORT void JNICALL Java_org_readium_sdk_lcp_Service_nativeInjectLicense(
        JNIEnv *env, jobject obj, jlong servicePtr, jstring jEpubPath, jstring jLicenseJson);
//
//JNIEXPORT void JNICALL Java_org_readium_sdk_lcp_Service_nativeInjectLicense(
//        JNIEnv *env, jobject obj, jlong servicePtr, jstring jEpubPath, jobject jLicense);

//
//#if !DISABLE_LSD
//
//JNIEXPORT void JNICALL Java_org_readium_sdk_lcp_Service_nativeNotifyLicenseStatusDocumentProcessingCancelled(
//        JNIEnv *env, jobject obj, jlong servicePtr);
//
//#endif //!DISABLE_LSD

#ifdef __cplusplus
}
#endif

#endif //LCP_ANDROID_SERVICE_H
