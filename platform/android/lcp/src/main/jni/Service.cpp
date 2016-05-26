// Copyright (c) 2016 Readium
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

#include "Service.h"
#include <public/ILicense.h>
#include <public/LcpStatus.h>
#include <public/ILcpService.h>

JNIEXPORT jobject JNICALL Java_org_readium_sdk_lcp_Service_nativeOpenLicense(
        JNIEnv *env, jobject obj, jlong servicePtr, jstring jLicenseJson) {
    const char * cLicenseJson = env->GetStringUTFChars(jLicenseJson, 0);
    std::string licenseJson(cLicenseJson);
    lcp::ILcpService * service = (lcp::ILcpService *) servicePtr;
    lcp::ILicense * license = nullptr;;
    lcp::Status status = service->OpenLicense(licenseJson, &license);

    jclass cls = env->FindClass("org/readium/sdk/lcp/License");
    jmethodID methodId = env->GetMethodID(cls, "<init>", "(JJ)V");
    return env->NewObject(cls, methodId, (jlong) license, (jlong) service);
}
