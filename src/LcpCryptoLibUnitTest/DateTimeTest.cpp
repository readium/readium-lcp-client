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
}