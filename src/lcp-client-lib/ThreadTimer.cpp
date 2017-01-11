// Copyright (c) 2016 Mantano
// Licensed to the Readium Foundation under one or more contributor license agreements.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation and/or
//    other materials provided with the distribution.
// 3. Neither the name of the organization nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#if FUTURE_ENABLED

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
            m_isRunning = true;
            m_worker = std::thread(&ThreadTimer::TimerThread, this);
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
            while (m_isAutoReset && m_isRunning);

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

#endif //FUTURE_ENABLED