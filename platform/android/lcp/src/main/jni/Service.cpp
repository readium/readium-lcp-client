//
// Created by clebeaupin on 05/02/16.
//

#include "Service.h"
#include <public/ILicense.h>
#include <public/LcpStatus.h>
#include <public/ILcpService.h>

// JNIEXPORT jobject JNICALL Java_org_readium_sdk_lcp_Service_nativeOpenLicense(
//         JNIEnv *env, jobject obj, jlong servicePtr, jstring jLicenseJson) {
//     const char * cLicenseJson = env->GetStringUTFChars(jLicenseJson, 0);
//     std::string licenseJson(cLicenseJson);
//     lcp::ILcpService * service = (lcp::ILcpService *) servicePtr;
//     lcp::ILicense * license = nullptr;;
//     lcp::Status status = service->OpenLicense(licenseJson, &license);

//     jclass cls = env->FindClass("org/readium/sdk/lcp/License");
//     jmethodID methodId = env->GetMethodID(cls, "<init>", "(JJ)V");
//     return env->NewObject(cls, methodId, (jlong) license, (jlong) service);
// }
