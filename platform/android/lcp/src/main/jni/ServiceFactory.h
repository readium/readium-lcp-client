//
// Created by clebeaupin on 05/02/16.
//

#ifndef LCP_ANDROID_SERVICE_FACTORY_H
#define LCP_ANDROID_SERVICE_FACTORY_H


#include <jni.h>
#include <public/ILcpService.h>
#include "StorageProvider.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace lcp {
    class ServiceFactory {
    public:
        ILcpService * build(const std::string &certPath, StorageProvider* storageProvider);
    };
}

JNIEXPORT jobject JNICALL Java_org_readium_sdk_lcp_ServiceFactory_nativeBuild(
        JNIEnv *env, jobject obj, jstring jCertContent, jobject jStorageProvider);


#ifdef __cplusplus
}
#endif

#endif //LCP_ANDROID_SERVICE_FACTORY_H