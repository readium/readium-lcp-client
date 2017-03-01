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


#include "License.h"
#include <public/LcpStatus.h>
#include <public/ILicense.h>
#include <public/IAcquistion.h>
#include <public/ILcpService.h>

//#include <ePub3/epub3.h>
//#include "epub3.h"
#include <public/lcp.h>

JNIEXPORT jboolean JNICALL Java_org_readium_sdk_lcp_License_nativeIsDecrypted(
        JNIEnv *env, jobject obj, jlong licensePtr) {
    lcp::ILicense * license = (lcp::ILicense *) licensePtr;
    return (license->Decrypted()) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jstring JNICALL Java_org_readium_sdk_lcp_License_nativeGetOriginalContent(
        JNIEnv *env, jobject obj, jlong licensePtr) {
    lcp::ILicense * license = (lcp::ILicense *) licensePtr;
    std::string c = license->OriginalContent();
    jstring res = env->NewStringUTF(c.c_str());
    //jstring res = toJstring(env, link.href.c_str(), false);
    return res;
}

JNIEXPORT jstring JNICALL Java_org_readium_sdk_lcp_License_nativeGetLinkPublication(
        JNIEnv *env, jobject obj, jlong licensePtr) {
    lcp::ILicense * license = (lcp::ILicense *) licensePtr;

    std::string name("publication");

    if (!license->Links()->Has(name)) {
        jstring res = env->NewStringUTF("");
        //jstring res = toJstring(env, "", false);
        return res;
    }

    lcp::Link link;
    license->Links()->GetLink(name, link);

    jstring res = env->NewStringUTF(link.href.c_str());
    //jstring res = toJstring(env, link.href.c_str(), false);
    return res;
}

JNIEXPORT jstring JNICALL Java_org_readium_sdk_lcp_License_nativeGetLinkStatus(
        JNIEnv *env, jobject obj, jlong licensePtr) {
    lcp::ILicense * license = (lcp::ILicense *) licensePtr;

    std::string name("status");

    if (!license->Links()->Has(name)) {
        jstring res = env->NewStringUTF("");
        //jstring res = toJstring(env, "", false);
        return res;
    }

    lcp::Link link;
    license->Links()->GetLink(name, link);

    jstring res = env->NewStringUTF(link.href.c_str());
    //jstring res = toJstring(env, link.href.c_str(), false);
    return res;
}


JNIEXPORT void JNICALL Java_org_readium_sdk_lcp_License_nativeDecrypt(
        JNIEnv *env, jobject obj, jlong licensePtr, jlong servicePtr, jstring jPassphrase) {
    const char * cPassphrase = env->GetStringUTFChars(jPassphrase, 0);
    std::string passphrase(cPassphrase);
    lcp::ILicense * license = (lcp::ILicense *) licensePtr;
    lcp::ILcpService * service = (lcp::ILcpService *) servicePtr;
    lcp::Status status = service->DecryptLicense(license, passphrase);
}

JNIEXPORT jboolean JNICALL Java_org_readium_sdk_lcp_License_nativeIsOlderThan(
        JNIEnv *env, jobject obj, jlong licensePtr, jlong servicePtr, jstring jTimestamp) {

    const char * cTimestamp = env->GetStringUTFChars(jTimestamp, 0);
    std::string t2(cTimestamp);

    lcp::ILicense * license = (lcp::ILicense *) licensePtr;
    lcp::ILcpService * service = (lcp::ILcpService *) servicePtr;

    std::string t1 = license->Updated();
    if (t1.length() == 0){
        t1 = license->Issued();
    }
    int compared = service->TimeStampCompare(t1, t2);

    return ((compared < 0) ? JNI_TRUE : JNI_FALSE);
}

JNIEXPORT void JNICALL Java_org_readium_sdk_lcp_License_nativeSetStatusDocumentProcessingFlag(
        JNIEnv *env, jobject obj, jlong licensePtr, jboolean jFlag) {

    lcp::ILicense * license = (lcp::ILicense *) licensePtr;
    bool flag = (jFlag == JNI_TRUE ? true : false);
    license->setStatusDocumentProcessingFlag(flag);
}

#if ENABLE_NET_PROVIDER_ACQUISITION
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
#endif //ENABLE_NET_PROVIDER_ACQUISITION
