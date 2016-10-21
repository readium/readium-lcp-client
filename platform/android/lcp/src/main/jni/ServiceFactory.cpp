//
// Created by clebeaupin on 05/02/16.
//

#include "ServiceFactory.h"
#include "Util.h"
#include "CredentialHandler.h"
#include "StatusDocumentHandler.h"
#include <public/DefaultFileSystemProvider.h>
#include <public/LcpServiceCreator.h>
#include <public/LcpContentFilter.h>
#include <public/LcpContentModule.h>

namespace lcp {
    ILcpService * ServiceFactory::build(
            const std::string &certContent,
            StorageProvider* storageProvider
#if ENABLE_NET_PROVIDER
            ,
        NetProvider* netProvider
#endif //ENABLE_NET_PROVIDER
    ) {
        DefaultFileSystemProvider * fileSystemProvider = new DefaultFileSystemProvider();
        LcpServiceCreator serviceCreator;
        ILcpService * service = nullptr;

        serviceCreator.CreateLcpService(
                certContent,
#if ENABLE_NET_PROVIDER
                netProvider,
#endif //ENABLE_NET_PROVIDER
                storageProvider,
                fileSystemProvider,
                &service);
        return service;
    }
}

JNIEXPORT jobject JNICALL Java_org_readium_sdk_lcp_ServiceFactory_nativeBuild(
        JNIEnv *env, jobject obj, jstring jCertContent,
        jobject jStorageProvider,
#if ENABLE_NET_PROVIDER
        jobject jNetProvider,
#endif //ENABLE_NET_PROVIDER
        jobject jCredentialHandler, jobject jStatusDocumentHandler
    ) {
    // Initialize jvm
    lcp::initJvm(env);
    const char * cCertContent = env->GetStringUTFChars(jCertContent, 0);
    std::string certContent(cCertContent);
    lcp::ServiceFactory serviceFactory;
    lcp::StorageProvider * storageProvider = new lcp::StorageProvider(jStorageProvider);
#if ENABLE_NET_PROVIDER
    lcp::NetProvider * netProvider = new lcp::NetProvider(jNetProvider);
#endif //ENABLE_NET_PROVIDER
    lcp::ILcpService * service = serviceFactory.build(certContent, storageProvider
#if ENABLE_NET_PROVIDER
            , netProvider
#endif //ENABLE_NET_PROVIDER
    );
    lcp::ICredentialHandler * credentialHandler = new lcp::CredentialHandler(jCredentialHandler, service);
    lcp::IStatusDocumentHandler * statusDocumentHandler = new lcp::StatusDocumentHandler(jStatusDocumentHandler, service);
    lcp::LcpContentModule::Register(service, credentialHandler, statusDocumentHandler);
    jclass cls = env->FindClass("org/readium/sdk/lcp/Service");
    jmethodID methodId = env->GetMethodID(cls, "<init>", "(J)V");
    return env->NewObject(cls, methodId, (jlong) service);
}