//
// Created by clebeaupin on 05/02/16.
//

#ifndef LCP_ANDROID_SERVICE_FACTORY_H
#define LCP_ANDROID_SERVICE_FACTORY_H


#include <jni.h>
#include <public/ILcpService.h>
#include "StorageProvider.h"

#if ENABLE_NET_PROVIDER
#include "NetProvider.h"
#endif //ENABLE_NET_PROVIDER

#ifdef __cplusplus
extern "C" {
#endif

namespace lcp {
    class ServiceFactory {
    public:
        ILcpService * build(const std::string &certPath, StorageProvider* storageProvider
#if ENABLE_NET_PROVIDER
                ,
                            NetProvider *netProvider
#endif //ENABLE_NET_PROVIDER
                            );

    };
}

JNIEXPORT jobject JNICALL Java_org_readium_sdk_lcp_ServiceFactory_nativeBuild(
        JNIEnv *env, jobject obj, jstring jCertContent,
        jobject jStorageProvider,
#if ENABLE_NET_PROVIDER
        jobject jNetProvider,
#endif //ENABLE_NET_PROVIDER
        jobject jCredentialHandler
#if !DISABLE_LSD
        , jobject jStatusDocumentHandler
#endif //!DISABLE_LSD
);


#ifdef __cplusplus
}
#endif

#endif //LCP_ANDROID_SERVICE_FACTORY_H