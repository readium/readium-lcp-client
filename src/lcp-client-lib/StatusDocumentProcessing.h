//
//#ifndef __LCP_LSD_PROCESSING_H__
//#define __LCP_LSD_PROCESSING_H__
//
//#if !DISABLE_LSD
//
//#include <string>
//#include <mutex>
//#include <memory>
//#include "public/IStatusDocumentProcessing.h"
//#include "public/IStatusDocumentProcessingCallback.h"
//#include "NonCopyable.h"
//
//namespace lcp
//{
//    class StatusDocumentProcessing : public IStatusDocumentProcessing, public NonCopyable
//    {
//    public:
//        StatusDocumentProcessing();
//
//        virtual Status Start(IStatusDocumentProcessingCallback * callback);
//        virtual Status Cancel();
//    private:
//        mutable std::mutex m_sync;
//        IStatusDocumentProcessingCallback * m_callback;
//        bool m_cancelled;
//    };
//}
//
//#endif //!DISABLE_LSD
//
//#endif //__LCP_LSD_PROCESSING_H__
