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

#ifndef __DATE_TIME_H__
#define __DATE_TIME_H__

#include <string>
#include "time64/time64.h"

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
