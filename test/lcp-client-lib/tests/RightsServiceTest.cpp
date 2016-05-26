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



#include <memory>
#include <gtest/gtest.h>
#include "public/lcp.h"
#include "TestStorageProvider.h"
#include "TestInfo.h"

namespace lcptest
{
    TEST(RightsServiceTest, RightsServiceTest)
    {
        TestStorageProvider storageProvider("storage.json", true);
        lcp::DefaultFileSystemProvider fsProvider;

        lcp::ILcpService * lcpServiceRaw = nullptr;
        lcp::LcpServiceCreator creator;
        lcp::Status res = creator.CreateLcpService(TestRootCertificate, nullptr, &storageProvider, nullptr, &lcpServiceRaw);
        std::unique_ptr<lcp::ILcpService> lcpService(lcpServiceRaw);

        ASSERT_EQ(lcp::StatusCode::ErrorCommonSuccess, res.Code);

        std::fstream mobyDickLicenseFile("..\\..\\..\\test\\lcp-client-lib\\data\\moby-dick-20120118.epub\\META-INF\\license.lcpl");
        std::string mobyDickLicenseStr(
            (std::istreambuf_iterator<char>(mobyDickLicenseFile)),
            std::istreambuf_iterator<char>()
            );

        lcp::ILicense * license = nullptr;
        res = lcpService->OpenLicense(mobyDickLicenseStr, &license);
        ASSERT_EQ(lcp::StatusCode::ErrorCommonSuccess, res.Code);
        res = lcpService->DecryptLicense(license, "White whales are huge!");
        ASSERT_EQ(lcp::StatusCode::ErrorCommonSuccess, res.Code);


        lcp::IRightsService * rightsService = lcpService->GetRightsService();
        ASSERT_TRUE(rightsService->CanUseRight(license, lcp::PrintRight));
        ASSERT_STREQ("-1", rightsService->GetValue(license, lcp::PrintRight).c_str());

        ASSERT_TRUE(rightsService->CanUseRight(license, lcp::CopyRight));
        ASSERT_STREQ("-1", rightsService->GetValue(license, lcp::CopyRight).c_str());

        ASSERT_TRUE(rightsService->CanUseRight(license, lcp::TtsRight));
        ASSERT_STREQ("true", rightsService->GetValue(license, lcp::TtsRight).c_str());

        ASSERT_TRUE(rightsService->UseRight(license, lcp::PrintRight, 200));
        ASSERT_STREQ("-1", rightsService->GetValue(license, lcp::PrintRight).c_str());

        ASSERT_TRUE(rightsService->UseRight(license, lcp::CopyRight, 500));
        ASSERT_STREQ("-1", rightsService->GetValue(license, lcp::CopyRight).c_str());

        rightsService->SetValue(license, lcp::PrintRight, "200000");
        ASSERT_TRUE(rightsService->CanUseRight(license, lcp::PrintRight));
        ASSERT_STREQ("200000", rightsService->GetValue(license, lcp::PrintRight).c_str());
        ASSERT_TRUE(rightsService->UseRight(license, lcp::PrintRight, 5658));
        ASSERT_STREQ("194342", rightsService->GetValue(license, lcp::PrintRight).c_str());

        rightsService->SetValue(license, "http://www.righttorewrite/com", "true");
        ASSERT_STREQ("true", rightsService->GetValue(license, "http://www.righttorewrite/com").c_str());

        rightsService->SetValue(license, lcp::StartRight, "2015-11-24T12:23:57Z");
        ASSERT_STREQ("2015-11-24T12:23:57Z", rightsService->GetValue(license, lcp::StartRight).c_str());

        rightsService->SetValue(license, lcp::EndRight, "2017-11-24T12:23:57Z");
        ASSERT_STREQ("2017-11-24T12:23:57Z", rightsService->GetValue(license, lcp::EndRight).c_str());

        storageProvider.Flush();

        // Check values after LCP Service reloading
        lcp::ILcpService * lcpServiceRawCheck = nullptr;
        res = creator.CreateLcpService(TestRootCertificate, nullptr, &storageProvider, nullptr, &lcpServiceRawCheck);
        lcpService.reset(lcpServiceRawCheck);
        ASSERT_EQ(lcp::StatusCode::ErrorCommonSuccess, res.Code);
        
        lcp::ILicense * licenseCheck = nullptr;
        res = lcpService->OpenLicense(mobyDickLicenseStr, &licenseCheck);
        ASSERT_EQ(lcp::StatusCode::ErrorCommonSuccess, res.Code);

        lcp::IRightsService * rightsServiceCheck = lcpService->GetRightsService();
       
        ASSERT_STREQ("true", rightsServiceCheck->GetValue(licenseCheck, lcp::TtsRight).c_str());
        ASSERT_STREQ("-1", rightsServiceCheck->GetValue(licenseCheck, lcp::CopyRight).c_str());
        ASSERT_STREQ("194342", rightsServiceCheck->GetValue(licenseCheck, lcp::PrintRight).c_str());
        ASSERT_STREQ("true", rightsServiceCheck->GetValue(licenseCheck, "http://www.righttorewrite/com").c_str());
        ASSERT_STREQ("2015-11-24T12:23:57Z", rightsServiceCheck->GetValue(licenseCheck, lcp::StartRight).c_str());
        ASSERT_STREQ("2017-11-24T12:23:57Z", rightsServiceCheck->GetValue(licenseCheck, lcp::EndRight).c_str());
    }
}