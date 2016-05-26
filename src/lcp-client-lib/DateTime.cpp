// Copyright (c) 2016 Mantano, Readium.
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



#include <sstream>
#include <iomanip>
#include <regex>
#include "DateTime.h"

namespace lcp
{
    /*static*/ std::string DateTime::IsoUtcFormat = "2015-11-11T22:21:37Z";
    /*static*/ std::string DateTime::IsoUtcFormatRegex = "\\d{4}-\\d{2}-\\d{2}T\\d{2}:\\d{2}:\\d{2}\\.*\\d*Z";
    /*static*/ std::string DateTime::IsoTimeZoneFormat = "2015-11-11T22:21:37+01:00";
    /*static*/ std::string DateTime::IsoTimeZoneFormatRegex = "\\d{4}-\\d{2}-\\d{2}T\\d{2}:\\d{2}:\\d{2}\\.*\\d*[+-]\\d{2}:\\d{2}";
    /*static*/ std::string DateTime::IsoJointUtcFormat = "20151111T222137Z";
    /*static*/ std::string DateTime::IsoJointUtcFormatRegex = "\\d{8}T\\d{6}Z";
    /*static*/ std::string DateTime::IsoJointTimeZoneFormat = "20151111T222137+0100";
    /*static*/ std::string DateTime::IsoJointTimeZoneFormatRegex = "\\d{8}T\\d{6}\\+\\d{4}";

    /*static*/ DateTime DateTime::Now()
    {
        DateTime utcNow;
        std::time_t currentTime = { 0 };
        std::time(&currentTime);
        utcNow.m_time = currentTime;
        TM * utcTmPtr = nullptr;
        utcTmPtr = gmtime64(&utcNow.m_time);
        utcNow.m_tm = *utcTmPtr;
        utcNow.m_time = mktime64(&utcNow.m_tm);
        return utcNow;
    }

    DateTime::DateTime()
    {
        m_tm = {};
        m_time = 0;
    }

    DateTime::DateTime(const std::string & isoTime)
        : m_isoTime(isoTime)
    {
        m_tm = {};
        m_time = 0;

        std::regex jointUtcRegex(IsoJointUtcFormatRegex);
        std::regex jointTimeZoneRegex(IsoJointTimeZoneFormatRegex);
        std::regex isoUtcRegex(IsoUtcFormatRegex);
        std::regex isoTimeZoneRegex(IsoTimeZoneFormatRegex);

        if (std::regex_match(isoTime, jointUtcRegex))
        {
            std::string normalIsoStr = JointToNormalIsoFormat(isoTime, true);
            m_isoTime = normalIsoStr;
            ProcessIsoUtcTime(m_isoTime);
        }
        else if (std::regex_match(isoTime, jointTimeZoneRegex))
        {
            std::string normalIsoStr = JointToNormalIsoFormat(isoTime, false);
            m_isoTime = normalIsoStr;
            ProcessIsoTimeZoneTime(m_isoTime);
        }
        else if (std::regex_match(isoTime, isoUtcRegex))
        {
            ProcessIsoUtcTime(m_isoTime);
        }
        else if (std::regex_match(isoTime, isoTimeZoneRegex))
        {
            ProcessIsoTimeZoneTime(m_isoTime);
        }
        else
        {
            throw std::invalid_argument("Wrong iso time format");
        }
    }

    DateTime::DateTime(const DateTime & right)
    {
        m_isoTime = right.m_isoTime;
        m_time = right.m_time;
        m_tm = right.m_tm;
    }

    DateTime & DateTime::operator= (const DateTime & right)
    {
        if (*this != right)
        {
            m_isoTime = right.m_isoTime;
            m_time = right.m_time;
            m_tm = right.m_tm;
        }
        return *this;
    }

    DateTime::DateTime(DateTime && right)
    {
        m_isoTime = right.m_isoTime;
        m_time = right.m_time;
        m_tm = right.m_tm;
        right.m_isoTime = "";
        right.m_time = 0;
        right.m_tm = {};
    }

    DateTime & DateTime::operator= (DateTime && right)
    {
        if (*this != right)
        {
            m_isoTime = right.m_isoTime;
            m_time = right.m_time;
            m_tm = right.m_tm;
            right.m_isoTime = "";
            right.m_time = 0;
            right.m_tm = {};
        }
        return *this;
    }

    bool DateTime::operator==(const DateTime & right)
    {
        return this->m_time == right.m_time;
    }

    bool DateTime::operator!=(const DateTime & right)
    {
        return !(*this == right);
    }

    bool DateTime::operator< (const DateTime & right)
    {
        return this->m_time < right.m_time;
    }

    bool DateTime::operator> (const DateTime & right)
    {
        return this->m_time > right.m_time;
    }

    std::string DateTime::ToString() const
    {
        if (m_isoTime.empty() && m_time != 0)
        {
            throw std::logic_error("Conversion to ISO string is not implemented");
        }
        return m_isoTime;
    }

    Time64_T DateTime::ToTime() const
    {
        return m_time;
    }

    TM DateTime::ToTm() const
    {
        return m_tm;
    }

    void DateTime::ProcessIsoUtcTime(const std::string & isoTime)
    {
        // Fraction of seconds cannot be parsed so remove it
        if (!TimeStringToTm(isoTime.substr(0, 19), "%Y-%m-%dT%H:%M:%S", m_tm))
        {
            throw std::runtime_error("Cannot parse iso time");
        }
        m_time = mktime64(&m_tm);
    }

    void DateTime::ProcessIsoTimeZoneTime(const std::string & isoTime)
    {
        size_t found = isoTime.find_last_of("+-");
        bool isPlus = (isoTime[found] == '+' ? true : false);
        std::string dateTimePart = isoTime.substr(0, found);
        std::string timeZonePart = isoTime.substr(found + 1);

        if (!TimeStringToTm(dateTimePart, "%Y-%m-%dT%H:%M:%S", m_tm))
        {
            throw std::runtime_error("Cannot parse iso time");
        }

        TM timeZone = {};
        if (!TimeStringToTm(timeZonePart, "%H:%M", timeZone))
        {
            throw std::runtime_error("Cannot parse iso time");
        }

        if (isPlus)
        {
            m_tm.tm_hour -= timeZone.tm_hour;
            m_tm.tm_min -= timeZone.tm_min;
        }
        else
        {
            m_tm.tm_hour += timeZone.tm_hour;
            m_tm.tm_min += timeZone.tm_min;
        }
        m_time = mktime64(&m_tm);
    }

    /*static*/ std::string DateTime::JointToNormalIsoFormat(const std::string & jointTime, bool isUtc)
    {
        std::string result(
            jointTime.substr(0, 4) + "-" +
            jointTime.substr(4, 2) + "-" +
            jointTime.substr(6, 5) + ":" +
            jointTime.substr(11,2) + ":"
            );

        if (isUtc)
        {
            result += jointTime.substr(13);
        }
        else
        {
            result += jointTime.substr(13, 5) + ":" + jointTime.substr(18);
        }
        return result;
    }

    /*static*/ bool DateTime::TimeStringToTm(const std::string & time, const std::string & format, TM & result)
    {
        result = {};
        std::tm tempResult = {};

        if (strptime(time.c_str(), format.c_str(), &tempResult) == NULL) {
            // Unable to convert time
            return false;
        }

        result = TmToTm64(tempResult);
        return true;
    }

    /*static*/ TM DateTime::TmToTm64(const std::tm & from)
    {
        TM res = {};
        res.tm_year = from.tm_year;
        res.tm_yday = from.tm_yday;
        res.tm_mon = from.tm_mon;
        res.tm_mday = from.tm_mday;
        res.tm_wday = from.tm_wday;
        res.tm_hour = from.tm_hour;
        res.tm_min = from.tm_min;
        res.tm_sec = from.tm_sec;
        res.tm_isdst = from.tm_isdst;
        return res;
    }
}