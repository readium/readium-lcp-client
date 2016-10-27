//
// Created by clebeaupin on 05/02/16.
//

#include "License.h"
#include <public/LcpStatus.h>
#include <public/ILicense.h>
#include <public/IAcquistion.h>
#include <public/ILcpService.h>
#include <public/IStatusDocumentProcessing.h>
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

#if ENABLE_NET_PROVIDER
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
#endif //ENABLE_NET_PROVIDER

JNIEXPORT jobject JNICALL Java_org_readium_sdk_lcp_License_nativeCreateStatusDocumentProcessing(
        JNIEnv *env, jobject obj, jlong licensePtr, jlong servicePtr, jstring jDstPath) {
    const char * cDstPath = env->GetStringUTFChars(jDstPath, 0);
    std::string dstPath(cDstPath);
    lcp::ILicense * license = (lcp::ILicense *) licensePtr;
    lcp::ILcpService * service = (lcp::ILcpService *) servicePtr;
    lcp::IStatusDocumentProcessing * statusDocumentProcessing;
    lcp::Status status = service->CreatePublicationStatusDocumentProcessing(dstPath, license, &statusDocumentProcessing);

    if (status.Code  != lcp::StatusCode::ErrorCommonSuccess) {
        return nullptr;
    }

    jclass cls = env->FindClass("org/readium/sdk/lcp/StatusDocumentProcessing");
    jmethodID methodId = env->GetMethodID(cls, "<init>", "(J)V");
    return env->NewObject(cls, methodId, (jlong) statusDocumentProcessing);
}
