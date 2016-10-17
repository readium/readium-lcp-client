
#ifndef __I_LSD_PROCESSING_CALLBACK_H__
#define __I_LSD_PROCESSING_CALLBACK_H__

#include "LcpStatus.h"

namespace lcp
{
    class IStatusDocumentProcessing;

    class IStatusDocumentProcessingCallback
    {
    public:
        virtual void OnStatusDocumentProcessingComplete(IStatusDocumentProcessing * acquisition) = 0;

        virtual ~IStatusDocumentProcessingCallback() {}
    };
}

#endif //__I_LSD_PROCESSING_CALLBACK_H__
