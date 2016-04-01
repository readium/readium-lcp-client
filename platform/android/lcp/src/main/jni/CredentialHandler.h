//
// Created by clebeaupin on 31/03/16.
//

#ifndef LCP_ANDROID_CREDENTIAL_HANDLER_H
#define LCP_ANDROID_CREDENTIAL_HANDLER_H

#include <string>
#include <jni.h>
#include <public/LcpContentModule.h>

#ifdef __cplusplus
extern "C" {
#endif

namespace lcp{
        class CredentialHandler: public ICredentialHandler {
        private:
            ILcpService * service;
            jobject jCredentialHandler;
            jmethodID jDecryptMethodId;
        public:
            CredentialHandler(jobject jCredentialHandler, ILcpService * service);
            ~CredentialHandler();
            void decrypt(ILicense *license);
        };
}

#ifdef __cplusplus
}
#endif

#endif // LCP_ANDROID_CREDENTIAL_HANDLER_H
