//
// Created by clebeaupin on 05/02/16.
//

#include "Service.h"
#include <public/ILicense.h>
#include <public/LcpStatus.h>
#include <public/ILcpService.h>

#if !DISABLE_LSD
#include <ePub3/utilities/utfstring.h>
#endif //!DISABLE_LSD

JNIEXPORT jobject JNICALL Java_org_readium_sdk_lcp_Service_nativeOpenLicense(
         JNIEnv *env, jobject obj, jlong servicePtr, jstring jLicenseJson) {
     const char * cLicenseJson = env->GetStringUTFChars(jLicenseJson, 0);
     std::string licenseJson(cLicenseJson);
     lcp::ILcpService * service = (lcp::ILcpService *) servicePtr;

     lcp::ILicense * license = nullptr;

     std::promise<lcp::ILicense*> licensePromise;
     auto licenseFuture = licensePromise.get_future();

#if !DISABLE_LSD
     const ePub3::string epubPath("");
#endif //!DISABLE_LSD

     lcp::Status status = service->OpenLicense(
#if !DISABLE_LSD
             epubPath,
#endif //!DISABLE_LSD
             licenseJson, licensePromise);

     if (status.Code == lcp::StatusCode::ErrorCommonSuccess) {

         // ...blocks until licensePromise.set_value(licensePtr)
         license = licenseFuture.get();
     }

     jclass cls = env->FindClass("org/readium/sdk/lcp/License");
     jmethodID methodId = env->GetMethodID(cls, "<init>", "(JJ)V");
     return env->NewObject(cls, methodId, (jlong) license, (jlong) service);
 }
