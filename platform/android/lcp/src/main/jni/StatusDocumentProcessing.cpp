
#include <public/IStatusDocumentProcessing.h>
#include <public/IStatusDocumentProcessingCallback.h>
#include "StatusDocumentProcessing.h"
#include "StatusDocumentProcessingCallback.h"


JNIEXPORT void JNICALL Java_org_readium_sdk_lcp_StatusDocumentProcessing_nativeStart(
        JNIEnv *env, jobject obj, jobject jStatusDocumentProcessingCallback, jlong statusDocumentProcessingPtr) {
    lcp::IStatusDocumentProcessing * statusDocumentProcessing = (lcp::IStatusDocumentProcessing *) statusDocumentProcessingPtr;
    lcp::IStatusDocumentProcessingCallback * statusDocumentProcessingCallback;

    if (jStatusDocumentProcessingCallback != nullptr) {
        statusDocumentProcessingCallback = new lcp::StatusDocumentProcessingCallback(jStatusDocumentProcessingCallback);
    }

    statusDocumentProcessing->Start(statusDocumentProcessingCallback);
}

JNIEXPORT void JNICALL Java_org_readium_sdk_lcp_StatusDocumentProcessing_nativeCancel(
        JNIEnv *env, jobject obj, jlong statusDocumentProcessingPtr) {
    lcp::IStatusDocumentProcessing * statusDocumentProcessing = (lcp::IStatusDocumentProcessing *) statusDocumentProcessingPtr;

    statusDocumentProcessing->Cancel();
}