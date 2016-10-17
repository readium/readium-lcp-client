
#ifndef __I_LSD_PROCESSING_H__
#define __I_LSD_PROCESSING_H__

#include <string>
#include "LcpStatus.h"

namespace lcp
{
    class IStatusDocumentProcessingCallback;

    // @see LcpService::CreatePublicationStatusDocumentProcessing()
    class IStatusDocumentProcessing
    {
    public:
        virtual Status Start(IStatusDocumentProcessingCallback * callback) = 0;

        virtual Status Cancel() = 0;

        virtual ~IStatusDocumentProcessing() {}
    };
}

#endif //__I_LSD_PROCESSING_H__
