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
            StorageProvider* storageProvider,
        NetProvider* netProvider
    ) {
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
        JNIEnv *env, jobject obj, jstring jCertContent,
        jobject jStorageProvider, jobject jNetProvider,
        jobject jCredentialHandler, jobject jStatusDocumentHandler
    ) {
    // Initialize jvm
    lcp::initJvm(env);
    const char * cCertContent = env->GetStringUTFChars(jCertContent, 0);
    std::string certContent(cCertContent);
    lcp::ServiceFactory serviceFactory;
    lcp::StorageProvider * storageProvider = new lcp::StorageProvider(jStorageProvider);
    lcp::NetProvider * netProvider = new lcp::NetProvider(jNetProvider);
    lcp::ILcpService * service = serviceFactory.build(certContent, storageProvider, netProvider);
    lcp::ICredentialHandler * credentialHandler = new lcp::CredentialHandler(jCredentialHandler, service);
    lcp::IStatusDocumentHandler * statusDocumentHandler = new lcp::StatusDocumentHandler(jStatusDocumentHandler, service);
    lcp::LcpContentModule::Register(service, credentialHandler, statusDocumentHandler);
    jclass cls = env->FindClass("org/readium/sdk/lcp/Service");
    jmethodID methodId = env->GetMethodID(cls, "<init>", "(J)V");
    return env->NewObject(cls, methodId, (jlong) service);
}