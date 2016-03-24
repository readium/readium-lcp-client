//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#include <gtest/gtest.h>
#include "TestInfo.h"
#include "public/lcp.h"
#include "JsonValueReader.h"
#include "JsonCanonicalizer.h"
#include "LcpUtils.h"

namespace lcptest
{
    TEST(JsonCanonicalizerTest, CanonicalizeLicenseSuccess)
    {
        std::fstream mobyDickLicenseFile("..\\..\\..\\test\\lcp-client-lib\\data\\moby-dick-20120118.epub\\META-INF\\license.lcpl");
        std::string mobyDickLicenseStr(
            (std::istreambuf_iterator<char>(mobyDickLicenseFile)),
            std::istreambuf_iterator<char>()
            );

        lcp::JsonValueReader reader;
        lcp::JsonCanonicalizer canonicalizer(mobyDickLicenseStr, &reader);
        ASSERT_STREQ(TestCanonicalJson.c_str(), canonicalizer.CanonicalLicense().c_str());
    }

    TEST(JsonCanonicalizerTest, CanonicalizeLicenseNotValid)
    {
        std::fstream mobyDickLicenseFile("..\\..\\..\\test\\lcp-client-lib\\data\\moby-dick-20120118.epub\\META-INF\\license.lcpl");
        std::string mobyDickLicenseStr(
            (std::istreambuf_iterator<char>(mobyDickLicenseFile)),
            std::istreambuf_iterator<char>()
            );

        lcp::JsonValueReader reader;
        try
        {
            lcp::JsonCanonicalizer canonicalizer(TestCanonicalJson, &reader);
        }
        catch (const lcp::StatusException & ex)
        {
            ASSERT_EQ(lcp::StatusCode::ErrorOpeningLicenseNotValid, ex.ResultStatus().Code);
        }
    }
}