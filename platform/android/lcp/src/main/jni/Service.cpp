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


#include "Service.h"
#include <public/ILicense.h>
#include <public/LcpStatus.h>
#include <LcpUtils.h>
#include <public/ILcpService.h>

#include <ePub3/utilities/utfstring.h>

//#include <ePub3/epub3.h>
extern "C" jboolean javaEPub3_handleSdkError(JNIEnv *env, jstring message, jboolean isSevereEpubError);
//extern "C" jstring toJstring(JNIEnv *env, const char* str, bool freeNative = false);


JNIEXPORT void JNICALL Java_org_readium_sdk_lcp_Service_nativeInjectLicense(
        JNIEnv *env, jobject obj, jlong servicePtr, jstring jEpubPath, jstring jLicenseJson) {

     lcp::ILcpService * service = (lcp::ILcpService *) servicePtr;

     const char * cEpubPath = env->GetStringUTFChars(jEpubPath, 0);
     std::string epubPath(cEpubPath);

     const char * cLicenseJson = env->GetStringUTFChars(jLicenseJson, 0);
     std::string licenseJson(cLicenseJson);

     service->InjectLicense(epubPath, licenseJson);
}
//
//JNIEXPORT void JNICALL Java_org_readium_sdk_lcp_Service_nativeInjectLicense(
//        JNIEnv *env, jobject obj, jlong servicePtr, jstring jEpubPath, jobject jLicense) {
//
//     lcp::ILcpService * service = (lcp::ILcpService *) servicePtr;
//
//     const char * cEpubPath = env->GetStringUTFChars(jEpubPath, 0);
//     std::string epubPath(cEpubPath);
//
////     jLicense = env->NewGlobalRef(jLicense);
//
//     jclass jLicenseClass = env->GetObjectClass(jLicense);
//     // TODO
//
//     service->InjectLicense(epubPath, license);
//
//     //env->DeleteGlobalRef(jLicense);
//}

JNIEXPORT jobject JNICALL Java_org_readium_sdk_lcp_Service_nativeOpenLicense(
        JNIEnv *env, jobject obj, jlong servicePtr, jstring jLicenseJson) {

     lcp::ILcpService * service = (lcp::ILcpService *) servicePtr;

     const char * cLicenseJson = env->GetStringUTFChars(jLicenseJson, 0);
     std::string licenseJson(cLicenseJson);

     lcp::ILicense* license = nullptr;
     lcp::ILicense** licensePTR = &license;

     const std::string epubPath("");

     try {
          lcp::Status status = service->OpenLicense(
                  epubPath,
                  licenseJson,
                  licensePTR);

          if (!lcp::Status::IsSuccess(status)) {

               //jstring test = toJstring(env, "test", false);
               jstring jmessage = env->NewStringUTF(lcp::Status::ToString(status).c_str());
               jboolean b = javaEPub3_handleSdkError(env, jmessage, (jboolean)true);
               env->DeleteLocalRef(jmessage);

               return nullptr;
          }
     } catch (lcp::StatusException &ex) {

          jstring jmessage = env->NewStringUTF(ex.what());
          jboolean b = javaEPub3_handleSdkError(env, jmessage, (jboolean)true);
          env->DeleteLocalRef(jmessage);

          return nullptr;
     }

     jclass cls = env->FindClass("org/readium/sdk/lcp/License");
     jmethodID methodId = env->GetMethodID(cls, "<init>", "(JJ)V");
     return env->NewObject(cls, methodId, (jlong) (*licensePTR), (jlong) service);
}
