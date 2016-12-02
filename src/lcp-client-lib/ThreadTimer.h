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

#ifndef __THREAD_TIMER_H__
#define __THREAD_TIMER_H__

#if FUTURE_ENABLED

#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include "NonCopyable.h"

namespace lcp
{
    class ThreadTimer : public NonCopyable
    {
    public:
        typedef std::chrono::system_clock ClockType;
        typedef ClockType::time_point TimePointType;
        typedef std::chrono::milliseconds DurationType;

        enum UsageTypeEnum
        {
            TimePointUsage,
            DurationUsage
        };

    public:
        explicit ThreadTimer(
            UsageTypeEnum usageType = DurationUsage,
            bool isAutoReset = false,
            std::function<void()> handler = std::function<void()>()
            );
        ~ThreadTimer();

        void Start();
        void Stop();
        bool IsRunning() const;
        void SetHandler(std::function<void()> handler);

        void SetUsage(UsageTypeEnum usageType);
        UsageTypeEnum UsageType() const;

        void SetTimePoint(const TimePointType & when);
        TimePointType TimePoint() const;

        void SetDuration(const DurationType & duration);
        DurationType Duration() const;

        void SetAutoReset(bool value);
        bool IsAutoReset() const;

        void RethrowExceptionIfAny();

    private:
        void TimerThread();

    private:
        std::thread m_worker;
        mutable std::mutex m_sync;
        std::condition_variable m_conditionRunning;
        std::exception_ptr m_currentException;

        std::function<void()> m_handler;
        TimePointType m_runTimePoint;
        DurationType m_runPeriod;

        bool m_isRunning;
        bool m_isAutoReset;
        UsageTypeEnum m_usageType;
    };
}

#endif //FUTURE_ENABLED

#endif //__THREAD_TIMER_H__