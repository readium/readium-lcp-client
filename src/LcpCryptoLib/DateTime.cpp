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
        if (!TimeStringToTm(isoTime, "%Y-%m-%dT%H:%M:%SZ", m_tm))
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
            m_tm.tm_hour += timeZone.tm_hour;
            m_tm.tm_min += timeZone.tm_min;
        }
        else
        {
            m_tm.tm_hour -= timeZone.tm_hour;
            m_tm.tm_min -= timeZone.tm_min;
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
        std::istringstream strm(time);
        strm >> std::get_time(&tempResult, format.c_str());
        result = TmToTm64(tempResult);
        return (!strm.fail());
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