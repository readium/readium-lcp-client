//
// Created by clebeaupin on 05/02/16.
//

#include "ServiceFactory.h"
#include "StorageProvider.h"
#include "NetProvider.h"
#include <public/DefaultFileSystemProvider.h>
#include <public/LcpServiceCreator.h>
#include <public/LcpContentFilter.h>
#include <public/ILcpService.h>
#include <string>

namespace lcp {
    ILcpService * ServiceFactory::build(const std::string &certContent, StorageProvider* storageProvider) {
        NetProvider * netProvider = new NetProvider();
        DefaultFileSystemProvider * fileSystemProvider = new DefaultFileSystemProvider();
        LcpServiceCreator serviceCreator;
        ILcpService * service = nullptr;

        serviceCreator.CreateLcpService(
                certContent,
                netProvider,
                storageProvider,
                fileSystemProvider,
                &service);
        return service;
    }
}

JNIEXPORT jobject JNICALL Java_org_readium_sdk_lcp_ServiceFactory_nativeBuild(
        JNIEnv *env, jobject obj, jstring jCertContent, jobject jStorageProvider
    ) {
    const char * cCertContent = env->GetStringUTFChars(jCertContent, 0);
    std::string certContent(cCertContent);
    lcp::ServiceFactory serviceFactory;
    lcp::StorageProvider *storageProvider = new lcp::StorageProvider(env, jStorageProvider);
    lcp::ILcpService * service = serviceFactory.build(certContent, storageProvider);
    lcp::LcpContentFilter::Register(service);
    jclass cls = env->FindClass("org/readium/sdk/lcp/Service");
    jmethodID methodId = env->GetMethodID(cls, "<init>", "(J)V");
    return env->NewObject(cls, methodId, (jlong) service);
}