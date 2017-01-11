
#ifndef LCP_ANDROID_LSD_HANDLER_H
#define LCP_ANDROID_LSD_HANDLER_H

#if !DISABLE_LSD

#include <string>
#include <jni.h>
#include <public/LcpContentModule.h>

#ifdef __cplusplus
extern "C" {
#endif

namespace lcp{
        class StatusDocumentHandler: public IStatusDocumentHandler {
        private:
            ILcpService * service;
            jobject jStatusDocumentHandler;
            jmethodID jProcessMethodId;
        public:
            StatusDocumentHandler(jobject jStatusDocumentHandler, ILcpService * service);
            ~StatusDocumentHandler();
            void process(ILicense *license);
        };
}

#ifdef __cplusplus
}
#endif

#endif //!DISABLE_LSD

#endif // LCP_ANDROID_LSD_HANDLER_H
