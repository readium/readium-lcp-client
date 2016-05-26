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

#include "License.h"
#include <public/LcpStatus.h>
#include <public/ILicense.h>
#include <public/IAcquistion.h>
#include <public/ILcpService.h>

JNIEXPORT jboolean JNICALL Java_org_readium_sdk_lcp_License_nativeIsDecrypted(
        JNIEnv *env, jobject obj, jlong licensePtr) {
    lcp::ILicense * license = (lcp::ILicense *) licensePtr;
    return (license->Decrypted()) ? JNI_TRUE : JNI_FALSE;
}


JNIEXPORT void JNICALL Java_org_readium_sdk_lcp_License_nativeDecrypt(
        JNIEnv *env, jobject obj, jlong licensePtr, jlong servicePtr, jstring jPassphrase) {
    const char * cPassphrase = env->GetStringUTFChars(jPassphrase, 0);
    std::string passphrase(cPassphrase);
    lcp::ILicense * license = (lcp::ILicense *) licensePtr;
    lcp::ILcpService * service = (lcp::ILcpService *) servicePtr;
    lcp::Status status = service->DecryptLicense(license, passphrase);
}

JNIEXPORT jobject JNICALL Java_org_readium_sdk_lcp_License_nativeCreateAcquisition(
        JNIEnv *env, jobject obj, jlong licensePtr, jlong servicePtr, jstring jDstPath) {
    const char * cDstPath = env->GetStringUTFChars(jDstPath, 0);
    std::string dstPath(cDstPath);
    lcp::ILicense * license = (lcp::ILicense *) licensePtr;
    lcp::ILcpService * service = (lcp::ILcpService *) servicePtr;
    lcp::IAcquisition * acquisition;
    lcp::Status status = service->CreatePublicationAcquisition(dstPath, license, &acquisition);

    if (status.Code  != lcp::StatusCode::ErrorCommonSuccess) {
        return nullptr;
    }

    jclass cls = env->FindClass("org/readium/sdk/lcp/Acquisition");
    jmethodID methodId = env->GetMethodID(cls, "<init>", "(J)V");
    return env->NewObject(cls, methodId, (jlong) acquisition);
}