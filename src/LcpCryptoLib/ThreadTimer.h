//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#ifndef __THREAD_TIMER_H__
#define __THREAD_TIMER_H__

#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace lcp
{
    class ThreadTimer
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

#endif //__THREAD_TIMER_H__