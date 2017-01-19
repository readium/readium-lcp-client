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
