//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

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
