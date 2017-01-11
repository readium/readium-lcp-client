//
// Created by clebeaupin on 05/02/16.
//

#ifndef LCP_ANDROID_LICENSE_H
#define LCP_ANDROID_LICENSE_H

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

namespace lcp {
    class License {

    };
}

JNIEXPORT jboolean JNICALL Java_org_readium_sdk_lcp_License_nativeIsDecrypted(
        JNIEnv *env, jobject obj, jlong licensePtr);

JNIEXPORT jstring JNICALL Java_org_readium_sdk_lcp_License_nativeGetOriginalContent(
        JNIEnv *env, jobject obj, jlong licensePtr);

JNIEXPORT jstring JNICALL Java_org_readium_sdk_lcp_License_nativeGetLinkPublication(
        JNIEnv *env, jobject obj, jlong licensePtr);

JNIEXPORT jstring JNICALL Java_org_readium_sdk_lcp_License_nativeGetLinkStatus(
        JNIEnv *env, jobject obj, jlong licensePtr);


JNIEXPORT void JNICALL Java_org_readium_sdk_lcp_License_nativeDecrypt(
        JNIEnv *env, jobject obj, jlong licensePtr, jlong servicePtr, jstring jPassphrase);

JNIEXPORT jboolean JNICALL Java_org_readium_sdk_lcp_License_nativeIsOlderThan(
        JNIEnv *env, jobject obj, jlong licensePtr, jlong servicePtr, jstring jTimestamp);

JNIEXPORT void JNICALL Java_org_readium_sdk_lcp_License_nativeSetStatusDocumentProcessingFlag(
        JNIEnv *env, jobject obj, jlong licensePtr, jboolean jFlag);

#if ENABLE_NET_PROVIDER_ACQUISITION
JNIEXPORT jobject JNICALL Java_org_readium_sdk_lcp_License_nativeCreateAcquisition(
        JNIEnv *env, jobject obj, jlong licensePtr, jlong servicePtr, jstring jDstPath);
#endif //ENABLE_NET_PROVIDER_ACQUISITION

#ifdef __cplusplus
}
#endif

#endif //LCP_ANDROID_LICENSE_H
