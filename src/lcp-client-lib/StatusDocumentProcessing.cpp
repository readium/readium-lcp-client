
#include "public/ILicense.h"
#include "StatusDocumentProcessing.h"
#include "IncludeMacros.h"
#include "LcpUtils.h"
#include <memory>
#include <sstream>

ZIPLIB_INCLUDE_START
#include "ziplib/Source/ZipLib/ZipFile.h"
ZIPLIB_INCLUDE_END

#include <thread>
#include <chrono>

namespace lcp
{
    StatusDocumentProcessing::StatusDocumentProcessing(
        ) :
            m_callback(nullptr)
    {
    }

    Status StatusDocumentProcessing::Start(IStatusDocumentProcessingCallback * callback)
    {
        m_callback = callback;

        std::this_thread::sleep_for(std::chrono::seconds(6));

        std::unique_lock<std::mutex> locker(m_sync);
        if (m_callback != nullptr)
        {
            m_callback->OnStatusDocumentProcessingComplete(this);
        }

        return Status(StatusCode::ErrorCommonSuccess);
    }

    Status StatusDocumentProcessing::Cancel()
    {
        return Status(StatusCode::ErrorCommonSuccess);
    }
}
