// Licensed to the Readium Foundation under one or more contributor license agreements.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation and/or
//    other materials provided with the distribution.
// 3. Neither the name of the organization nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


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
#if !DISABLE_NET_PROVIDER
            , NetProvider* netProvider
#endif //!DISABLE_NET_PROVIDER
    ) {
        DefaultFileSystemProvider * fileSystemProvider = new DefaultFileSystemProvider();
        LcpServiceCreator serviceCreator;
        ILcpService * service = nullptr;

        serviceCreator.CreateLcpService(
                certContent,
#if !DISABLE_NET_PROVIDER
                netProvider,
#endif //!DISABLE_NET_PROVIDER
                storageProvider,
                fileSystemProvider,
                &service);
        return service;
    }
}

JNIEXPORT jobject JNICALL Java_org_readium_sdk_lcp_ServiceFactory_nativeBuild(
        JNIEnv *env, jobject obj, jstring jCertContent,
        jobject jStorageProvider,
#if !DISABLE_NET_PROVIDER
        jobject jNetProvider,
#endif //!DISABLE_NET_PROVIDER
        jobject jCredentialHandler, jobject jStatusDocumentHandler
    ) {
    // Initialize jvm
    lcp::initJvm(env);
    const char * cCertContent = env->GetStringUTFChars(jCertContent, 0);
    std::string certContent(cCertContent);
    lcp::ServiceFactory serviceFactory;
    lcp::StorageProvider * storageProvider = new lcp::StorageProvider(jStorageProvider);
#if !DISABLE_NET_PROVIDER
    lcp::NetProvider * netProvider = new lcp::NetProvider(jNetProvider);
#endif //!DISABLE_NET_PROVIDER
    lcp::ILcpService * service = serviceFactory.build(certContent, storageProvider
#if !DISABLE_NET_PROVIDER
            , netProvider
#endif //!DISABLE_NET_PROVIDER
    );
    lcp::ICredentialHandler * credentialHandler = new lcp::CredentialHandler(jCredentialHandler, service);
    lcp::IStatusDocumentHandler * statusDocumentHandler = new lcp::StatusDocumentHandler(jStatusDocumentHandler, service);
    lcp::LcpContentModule::Register(service, credentialHandler, statusDocumentHandler);
    jclass cls = env->FindClass("org/readium/sdk/lcp/Service");
    jmethodID methodId = env->GetMethodID(cls, "<init>", "(J)V");
    return env->NewObject(cls, methodId, (jlong) service);
}