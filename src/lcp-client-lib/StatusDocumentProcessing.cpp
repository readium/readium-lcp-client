
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
    StatusDocumentProcessing::StatusDocumentProcessing() : m_callback(nullptr), m_cancelled(false)
    {
    }

    Status StatusDocumentProcessing::Start(IStatusDocumentProcessingCallback * callback)
    {
        m_callback = callback;

        // TODO: remove this (this is just to emulate latency, and see if the app UI does not block, as expected)
        std::this_thread::sleep_for(std::chrono::seconds(6));

        if (m_cancelled) {
            return Status(StatusCode::ErrorCommonSuccess);
        }

        std::unique_lock<std::mutex> locker(m_sync);
        if (m_callback != nullptr)
        {
            m_callback->OnStatusDocumentProcessingComplete(this);
        }

        return Status(StatusCode::ErrorCommonSuccess);
    }

    Status StatusDocumentProcessing::Cancel()
    {
        // TODO: although the cancellation is handled on the app side (in the OnStatusDocumentProcessingComplete() callback handler), here we can cut short any significant operation that runs in the background.
        m_cancelled = true;
        return Status(StatusCode::ErrorCommonSuccess);
    }
}
