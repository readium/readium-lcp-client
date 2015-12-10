//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#include "ThreadTimer.h"

namespace lcp
{
    ThreadTimer::ThreadTimer(UsageTypeEnum usageType, bool isAutoReset, std::function<void()> handler)
        : m_isRunning(false)
        , m_currentException(nullptr)
        , m_usageType(usageType)
        , m_isAutoReset(isAutoReset)
        , m_handler(handler)
    {
        m_runPeriod = DurationType::zero();
        m_runTimePoint = ClockType::now();
    }

    ThreadTimer::~ThreadTimer()
    {
        this->Stop();
    }

    void ThreadTimer::Start()
    {
        std::unique_lock<std::mutex> locker(m_sync);
        if (!m_isRunning)
        {
            m_worker = std::thread(&ThreadTimer::TimerThread, this);
            m_isRunning = true;
        }
    }

    void ThreadTimer::Stop()
    {
        std::unique_lock<std::mutex> locker(m_sync);
        if (m_isRunning)
        {
            m_isRunning = false;
            m_conditionRunning.notify_one();
            locker.unlock();

            if (m_worker.joinable())
            {
                m_worker.join();
            }
        }
    }

    bool ThreadTimer::IsRunning() const
    {
        std::unique_lock<std::mutex> locker(m_sync);
        return m_isRunning;
    }

    void ThreadTimer::SetUsage(UsageTypeEnum usageType)
    {
        std::unique_lock<std::mutex> locker(m_sync);
        m_usageType = usageType;
    }

    ThreadTimer::UsageTypeEnum ThreadTimer::UsageType() const
    {
        std::unique_lock<std::mutex> locker(m_sync);
        return m_usageType;
    }    

    void ThreadTimer::SetHandler(std::function<void()> handler)
    {
        std::unique_lock<std::mutex> locker(m_sync);
        m_handler = handler;
    }

    void ThreadTimer::SetTimePoint(const TimePointType & when)
    {
        std::unique_lock<std::mutex> locker(m_sync);
        m_runTimePoint = when;
    }

    ThreadTimer::TimePointType ThreadTimer::TimePoint() const
    {
        std::unique_lock<std::mutex> locker(m_sync);
        return m_runTimePoint;
    }

    void ThreadTimer::SetDuration(const DurationType & duration)
    {
        std::unique_lock<std::mutex> locker(m_sync);
        m_runPeriod = duration;
    }

    ThreadTimer::DurationType ThreadTimer::Duration() const
    {
        std::unique_lock<std::mutex> locker(m_sync);
        return m_runPeriod;
    }

    void ThreadTimer::SetAutoReset(bool value)
    {
        std::unique_lock<std::mutex> locker(m_sync);
        m_isAutoReset = value;
    }

    bool ThreadTimer::IsAutoReset() const
    {
        std::unique_lock<std::mutex> locker(m_sync);
        return m_isAutoReset;
    }

    void ThreadTimer::TimerThread()
    {
        try
        {
            std::unique_lock<std::mutex> locker(m_sync);
            do
            {
                if (m_usageType == UsageTypeEnum::TimePointUsage)
                {
                    m_conditionRunning.wait_until(locker, m_runTimePoint, [&]() { return !m_isRunning; });
                }
                else if (m_usageType == UsageTypeEnum::DurationUsage)
                {
                    m_conditionRunning.wait_for(locker, m_runPeriod, [&]() { return !m_isRunning; });
                }
                else
                {
                    throw std::logic_error("Unknown timer usage type");
                }

                locker.unlock();
                if (m_handler)
                {
                    m_handler();
                }
                locker.lock();
            }
            while (m_isAutoReset);

            // One-time shot, thread exits after first loop cycle
            if (m_isRunning)
            {
                m_isRunning = false;
            }
        }
        catch (const std::exception & ex)
        {
            std::unique_lock<std::mutex> locker(m_sync);
            m_currentException = std::make_exception_ptr(ex);
        }
    }

    void ThreadTimer::RethrowExceptionIfAny()
    {
        std::unique_lock<std::mutex> locker(m_sync);
        if (m_currentException != nullptr)
        {
            std::rethrow_exception(m_currentException);
        }
    }
}