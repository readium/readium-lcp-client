
#include "StatusDocumentHandler.h"
#include "Util.h"

namespace lcp {
    StatusDocumentHandler::StatusDocumentHandler(jobject jStatusDocumentHandler, ILcpService * service) {
        JNIEnv *env = getJNIEnv();
        this->jStatusDocumentHandler = env->NewGlobalRef(jStatusDocumentHandler);
        jclass jStatusDocumentHandlerClass = env->GetObjectClass(this->jStatusDocumentHandler);
        this->jProcessMethodId = env->GetMethodID(jStatusDocumentHandlerClass, "process",
                                                   "(Lorg/readium/sdk/lcp/License;)V");
        this->service = service;
    }

    StatusDocumentHandler::~StatusDocumentHandler() {
        JNIEnv *env = getJNIEnv();
        env->DeleteGlobalRef(this->jStatusDocumentHandler);
    }

    void StatusDocumentHandler::process(ILicense *license) {
        JNIEnv *env = getJNIEnv();
        jclass cls = env->FindClass("org/readium/sdk/lcp/License");
        jmethodID methodId = env->GetMethodID(cls, "<init>", "(JJ)V");
        jobject jLicense = env->NewObject(cls, methodId, (jlong) license, (jlong) this->service);
        env->CallVoidMethod(this->jStatusDocumentHandler, this->jProcessMethodId, jLicense);
    }
}
