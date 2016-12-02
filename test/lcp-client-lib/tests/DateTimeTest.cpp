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


#include <gtest/gtest.h>
#include "DateTime.h"

namespace lcptest
{
    TEST(DateTimeTest, JointUtcToNormalIso)
    {
        std::string joint = "20151111T222137Z";
        lcp::DateTime object(joint);
        ASSERT_EQ("2015-11-11T22:21:37Z", object.ToString());
    }

    TEST(DateTimeTest, JointTimeZoneToNormalIso)
    {
        std::string joint = "20151111T222137+0100";
        lcp::DateTime object(joint);
        ASSERT_EQ("2015-11-11T22:21:37+01:00", object.ToString());
    }

    TEST(DateTimeTest, JointIso_Utc_WrongFormatThrow)
    {
        std::string joint1 = "20151111T222137Z-";
        ASSERT_THROW(lcp::DateTime object(joint1), std::invalid_argument);

        std::string joint2 = "20151111T22A137Z";
        ASSERT_THROW(lcp::DateTime object(joint2), std::invalid_argument);

        std::string joint3 = "20151111222137Z";
        ASSERT_THROW(lcp::DateTime object(joint3), std::invalid_argument);
    }

    TEST(DateTimeTest, JointIsoFormat_TimeZone_WrongFormatThrow)
    {
        std::string joint1 = "20151111T222137+0100-";
        ASSERT_THROW(lcp::DateTime object(joint1), std::invalid_argument);

        std::string joint2 = "201y1111T222137+0100";
        ASSERT_THROW(lcp::DateTime object(joint2), std::invalid_argument);

        std::string joint3 = "20151111T2A2137+0100";
        ASSERT_THROW(lcp::DateTime object(joint3), std::invalid_argument);
    }

    TEST(DateTimeTest, NormalIso_Utc_WrongFormatThrow)
    {
        std::string joint1 = "2015-11-11T22:21:37X";
        ASSERT_THROW(lcp::DateTime object(joint1), std::invalid_argument);

        std::string joint2 = "2015-11-11T22 21 37Z";
        ASSERT_THROW(lcp::DateTime object(joint2), std::invalid_argument);

        std::string joint3 = "2015-11-11T22-21-37Z";
        ASSERT_THROW(lcp::DateTime object(joint3), std::invalid_argument);

        std::string joint4 = "2015-AA-11T22:21:37Z";
        ASSERT_THROW(lcp::DateTime object(joint4), std::invalid_argument);
    }

    TEST(DateTimeTest, NormalIsoFormat_TimeZone_WrongFormatThrow)
    {
        std::string joint1 = "2015-11-11T22:21:37+";
        ASSERT_THROW(lcp::DateTime object(joint1), std::invalid_argument);

        std::string joint2 = "2015-11-11T22 21 37Z+01:00";
        ASSERT_THROW(lcp::DateTime object(joint2), std::invalid_argument);

        std::string joint3 = "2015-11-11T22-21-37Z+01:00";
        ASSERT_THROW(lcp::DateTime object(joint3), std::invalid_argument);

        std::string joint4 = "2015-AA-11T22:21:37Z+01:00";
        ASSERT_THROW(lcp::DateTime object(joint4), std::invalid_argument);
    }

    TEST(DateTimeTest, NormalIsoTimeToTime_t_Utc)
    {
        std::string timeStr = "2015-11-11T22:21:37Z";
        lcp::DateTime object(timeStr);
        ASSERT_EQ(object.ToTime(), 1447273297);
    }

    TEST(DateTimeTest, NormalIsoTimeToTime_t_TimeZoneZero)
    {
        std::string timeStr = "2015-11-11T22:21:37+00:00";
        lcp::DateTime object(timeStr);
        ASSERT_EQ(object.ToTime(), 1447273297);
    }

    TEST(DateTimeTest, NormalIsoTimeToTime_t_TimeZonePlusTwo)
    {
        std::string timeStr = "2015-11-11T22:21:37+02:00";
        lcp::DateTime object(timeStr);
        ASSERT_EQ(object.ToTime(), 1447280497);
    }

    TEST(DateTimeTest, NormalIsoTimeToTime_t_TimeZoneMinusTwo)
    {
        std::string timeStr = "2015-11-11T22:21:37-02:00";
        lcp::DateTime object(timeStr);
        ASSERT_EQ(object.ToTime(), 1447266097);
    }

    TEST(DateTimeTest, CompareSmaller)
    {
        lcp::DateTime left("2015-11-11T22:21:37Z");
        lcp::DateTime right("2015-11-11T22:21:37+02:00");
        ASSERT_TRUE(left < right);
    }

    TEST(DateTimeTest, CompareBigger)
    {
        lcp::DateTime left("2015-11-11T22:21:37+02:00");
        lcp::DateTime right("2015-11-11T22:21:37Z");
        ASSERT_TRUE(left > right);
    }

    TEST(DateTimeTest, Equality)
    {
        lcp::DateTime left("2015-11-11T22:21:37+00:00");
        lcp::DateTime right("2015-11-11T22:21:37Z");
        ASSERT_TRUE(left == right);
    }

    TEST(DateTimeTest, NotEqual)
    {
        lcp::DateTime left("2015-11-11T22:21:37+05:00");
        lcp::DateTime right("2015-11-11T22:21:37Z");
        ASSERT_TRUE(left != right);
    }

    TEST(DateTimeTest, DateWithFractionalSecondsPart)
    {
        std::string timeStr = "2015-11-11T22:21:37.984546-02:00";
        lcp::DateTime object(timeStr);
        ASSERT_EQ(object.ToTime(), 1447266097);
        ASSERT_STREQ("2015-11-11T22:21:37.984546-02:00", object.ToString().c_str());
    }

    TEST(DateTimeTest, 2038YearBugIn32BitSystemsTest)
    {
        std::string timeStr = "2045-11-11T22:21:37-02:00";
        lcp::DateTime object(timeStr);
        ASSERT_EQ(object.ToTime(), 2394037297);
        ASSERT_STREQ("2045-11-11T22:21:37-02:00", object.ToString().c_str());
    }

    TEST(DateTimeTest, DateTimeNow)
    {
        lcp::DateTime now = lcp::DateTime::Now();
    }
}