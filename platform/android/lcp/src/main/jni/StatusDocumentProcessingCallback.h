//
//#ifndef LCP_ANDROID_LSD_PROCESSING_CALLBACK_H
//#define LCP_ANDROID_LSD_PROCESSING_CALLBACK_H
//
//#if !DISABLE_LSD
//
//#include <jni.h>
//#include <public/IStatusDocumentProcessingCallback.h>
//
//#ifdef __cplusplus
//extern "C" {
//#endif
//
//namespace lcp {
//    class StatusDocumentProcessingCallback: public IStatusDocumentProcessingCallback {
//    private:
//        jobject jStatusDocumentProcessingCallback;
//        jmethodID jOnStatusDocumentProcessingCompleteMethodId;
//
//    public:
//        StatusDocumentProcessingCallback(jobject jStatusDocumentProcessingCallback);
//        ~StatusDocumentProcessingCallback();
//        void OnStatusDocumentProcessingComplete(IStatusDocumentProcessing * statusDocumentProcessing);
//    };
//}
//
//#ifdef __cplusplus
//}
//#endif
//
//#endif //!DISABLE_LSD
//
//#endif //LCP_ANDROID_LSD_PROCESSING_CALLBACK_H