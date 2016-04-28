//
// Created by clebeaupin on 31/03/16.
//

#include "CredentialHandler.h"
#include "Util.h"

namespace lcp {
    CredentialHandler::CredentialHandler(jobject jCredentialHandler, ILcpService * service) {
        JNIEnv *env = getJNIEnv();
        this->jCredentialHandler = env->NewGlobalRef(jCredentialHandler);
        jclass jCredentialHandlerClass = env->GetObjectClass(this->jCredentialHandler);
        this->jDecryptMethodId = env->GetMethodID(jCredentialHandlerClass, "decrypt",
                                                   "(Lorg/readium/sdk/lcp/License;)V");
        this->service = service;
    }

    CredentialHandler::~CredentialHandler() {
        JNIEnv *env = getJNIEnv();
        env->DeleteGlobalRef(this->jCredentialHandler);
    }

    void CredentialHandler::decrypt(ILicense *license) {
        JNIEnv *env = getJNIEnv();
        jclass cls = env->FindClass("org/readium/sdk/lcp/License");
        jmethodID methodId = env->GetMethodID(cls, "<init>", "(JJ)V");
        jobject jLicense = env->NewObject(cls, methodId, (jlong) license, (jlong) this->service);
        env->CallVoidMethod(this->jCredentialHandler, this->jDecryptMethodId, jLicense);
    }
}