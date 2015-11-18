#include <sstream>
#include <iomanip>
#include <ctime>
#include "DateTime.h"
#include <regex>

namespace lcp
{
    /*static*/ std::string DateTime::IsoUtcFormat = "2015-11-11T22:21:37Z";
    /*static*/ std::string DateTime::IsoUtcFormatRegex = "\\d{4}-\\d{2}-\\d{2}T\\d{2}:\\d{2}:\\d{2}Z";
    /*static*/ std::string DateTime::IsoTimeZoneFormat = "2015-11-11T22:21:37+01:00";
    /*static*/ std::string DateTime::IsoTimeZoneFormatRegex = "\\d{4}-\\d{2}-\\d{2}T\\d{2}:\\d{2}:\\d{2}[+-]\\d{2}:\\d{2}";
    /*static*/ std::string DateTime::IsoJointUtcFormat = "20151111T222137Z";
    /*static*/ std::string DateTime::IsoJointUtcFormatRegex = "\\d{8}T\\d{6}Z";
    /*static*/ std::string DateTime::IsoJointTimeZoneFormat = "20151111T222137+0100";
    /*static*/ std::string DateTime::IsoJointTimeZoneFormatRegex = "\\d{8}T\\d{6}\\+\\d{4}";

    /*static*/ DateTime DateTime::Now()
    {
        DateTime utcNow;
        std::time(&utcNow.m_time);
        std::tm * utcTmPtr = nullptr;
        utcTmPtr = std::gmtime(&utcNow.m_time);
        utcNow.m_tm = *utcTmPtr;
        utcNow.m_time = std::mktime(&utcNow.m_tm);
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
        return std::difftime(this->m_time, right.m_time) == 0;
    }

    bool DateTime::operator!=(const DateTime & right)
    {
        return !(*this == right);
    }

    bool DateTime::operator< (const DateTime & right)
    {
        return std::difftime(this->m_time, right.m_time) < 0;
    }

    bool DateTime::operator> (const DateTime & right)
    {
        return std::difftime(this->m_time, right.m_time) > 0;
    }

    std::string DateTime::ToString() const
    {
        if (m_isoTime.empty() && m_time != 0)
        {
            throw std::logic_error("Conversion to ISO string is not implemented");
        }
        return m_isoTime;
    }

    std::time_t DateTime::ToTime() const
    {
        return m_time;
    }

    std::tm DateTime::ToTm() const
    {
        return m_tm;
    }

    void DateTime::ProcessIsoUtcTime(const std::string & isoTime)
    {
        if (!TimeStringToTm(isoTime, "%Y-%m-%dT%H:%M:%SZ", m_tm))
        {
            throw std::runtime_error("Cannot parse iso time");
        }
        m_time = std::mktime(&m_tm);
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

        std::tm timeZone = {};
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
        m_time = std::mktime(&m_tm);
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

    /*static*/ bool DateTime::TimeStringToTm(const std::string & time, const std::string & format, std::tm & result)
    {
        result = {};
        std::istringstream strm(time);
        strm >> std::get_time(&result, format.c_str());
        return (!strm.fail());
    }

}