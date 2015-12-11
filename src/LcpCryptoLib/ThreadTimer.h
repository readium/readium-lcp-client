//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//  Licensed under Gnu Affero General Public License Version 3 (provided, notwithstanding this notice,
//  Readium Foundation reserves the right to license this material under a different separate license,
//  and if you have done so, the terms of that separate license control and the following references
//  to GPL do not apply).
//
//  This program is free software: you can redistribute it and/or modify it under the terms of the GNU
//  Affero General Public License as published by the Free Software Foundation, either version 3 of
//  the License, or (at your option) any later version. You should have received a copy of the GNU
//  Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
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