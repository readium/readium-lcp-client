#ifndef __DATE_TIME_H__
#define __DATE_TIME_H__

#include <string>
#include <ctime>

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
        std::time_t ToTime() const;
        std::tm ToTm() const;
        
        bool operator==(const DateTime & right);
        bool operator!=(const DateTime & right);
        bool operator< (const DateTime & right);
        bool operator> (const DateTime & right);

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
        static bool TimeStringToTm(const std::string & time, const std::string & format, std::tm & result);

    private:
        std::string m_isoTime;
        std::time_t m_time;
        std::tm m_tm;
    };

}

#endif //__DATE_TIME_H__
