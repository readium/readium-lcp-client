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
    ILcpService * ServiceFactory::build(const std::string &certPath) {
        StorageProvider * storageProvider = new StorageProvider();
        NetProvider * netProvider = new NetProvider();
        DefaultFileSystemProvider * fileSystemProvider = new DefaultFileSystemProvider();
        LcpServiceCreator serviceCreator;
        ILcpService * service = nullptr;

        serviceCreator.CreateLcpService(
                certPath,
                netProvider,
                storageProvider,
                fileSystemProvider,
                &service);
        return service;
    }
}

JNIEXPORT jobject JNICALL Java_org_readium_sdk_lcp_ServiceFactory_build(JNIEnv *env,
                                                                        jobject obj,
                                                                        jstring jCertPath) {
    const char * cCertPath = env->GetStringUTFChars(jCertPath, 0);
    std::string certPath(cCertPath);
    lcp::ServiceFactory serviceFactory;
    lcp::ILcpService * service = serviceFactory.build(certPath);
    lcp::LcpContentFilter::Register(service);
    jclass cls = env->FindClass("org/readium/sdk/lcp/Service");
    jmethodID methodId = env->GetMethodID(cls, "<init>", "(J)V");
    return env->NewObject(cls, methodId, (jlong) service);
}