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


#ifndef __DATE_TIME_H__
#define __DATE_TIME_H__

#include "time64/time64.h"
#include <string>

namespace lcp
{
    class DateTime
    {
    public:
        DateTime();
        explicit DateTime(const std::string & isoTime);
        
        explicit DateTime(const DateTime & right);
        DateTime & operator= (const DateTime & right);
        DateTime(DateTime && right);
        DateTime & operator= (DateTime && right);

        std::string ToString() const;
        Time64_T ToTime() const;
        TM ToTm() const;
        
        bool operator==(const DateTime & right);
        bool operator!=(const DateTime & right);
        bool operator< (const DateTime & right);
        bool operator> (const DateTime & right);

        static DateTime Now();

    public:
        static std::string IsoUtcFormat;
        static std::string IsoTimeZoneFormat;
        static std::string IsoJointUtcFormat;
        static std::string IsoJointTimeZoneFormat;
        static std::string IsoUtcFormatRegex;
        static std::string IsoTimeZoneFormatRegex;
        static std::string IsoJointUtcFormatRegex;
        static std::string IsoJointTimeZoneFormatRegex;

    private:
        void ProcessIsoUtcTime(const std::string & isoTime);
        void ProcessIsoTimeZoneTime(const std::string & isoTime);

        static std::string JointToNormalIsoFormat(const std::string & jointTime, bool isUtc);
        static bool TimeStringToTm(const std::string & time, const std::string & format, TM & result);

        static TM TmToTm64(const std::tm & from);

    private:
        std::string m_isoTime;
        Time64_T m_time;
        TM m_tm;
    };

}

#endif //__DATE_TIME_H__
