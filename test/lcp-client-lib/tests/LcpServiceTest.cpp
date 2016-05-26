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
#include "TestInfo.h"
#include "TestStorageProvider.h"

namespace lcptest
{
    class LcpServiceTest : public ::testing::Test
    {
    protected:
        void SetUp()
        {
            m_storageProvider.reset(new TestStorageProvider("storage.json", true));

            lcp::ILcpService * lcpServiceRaw = nullptr;
            lcp::LcpServiceCreator creator;
            lcp::Status res = creator.CreateLcpService(TestRootCertificate, nullptr, m_storageProvider.get(), nullptr, &lcpServiceRaw);
            if (!lcp::Status::IsSuccess(res))
            {
                throw std::runtime_error("can not create ILcpService");
            }
            m_lcpService.reset(lcpServiceRaw);
        }
        void TearDown()
        {
            m_lcpService.reset();
        }

    protected:
        lcp::DefaultFileSystemProvider m_fsProvider;
        std::unique_ptr<TestStorageProvider> m_storageProvider;
        std::unique_ptr<lcp::ILcpService> m_lcpService;
    };

    TEST_F(LcpServiceTest, LcpServiceOpen_CheckLicenseParsing)
    {
        std::fstream mobyDickLicenseFile("..\\..\\..\\test\\lcp-client-lib\\data\\moby-dick-20120118.epub\\META-INF\\license.lcpl");
        std::string mobyDickLicenseStr(
            (std::istreambuf_iterator<char>(mobyDickLicenseFile)),
            std::istreambuf_iterator<char>()
            );

        lcp::ILicense * license = nullptr;
        lcp::Status res = m_lcpService->OpenLicense(mobyDickLicenseStr, &license);
        ASSERT_EQ(lcp::StatusCode::ErrorCommonSuccess, res.Code);


        ASSERT_STREQ(TestCanonicalJson.c_str(), license->CanonicalContent().c_str());
        ASSERT_STREQ("df09ac25-a386-4c5c-b167-33ce4c36ca65", license->Id().c_str());
        ASSERT_STREQ("http://example.com", license->Provider().c_str());
        ASSERT_STREQ("2015-11-24T13:32:39.905384033+01:00", license->Issued().c_str());
        ASSERT_STREQ("2015-11-24T13:32:39.905384033+01:00", license->Updated().c_str());
        ASSERT_FALSE(license->Decrypted());

        ASSERT_STREQ("http://readium.org/lcp/profile-1.0", license->Crypto()->EncryptionProfile().c_str());
        ASSERT_STREQ("7GMxrWNzUFpcwRNhclNECE3ZxvODsBt13N52zBoWdd6lQSNITVEJBXKxencovnPBBthBgE/6UE3sEmZYSfMrkQ==", license->Crypto()->ContentKey().c_str());
        ASSERT_STREQ("http://www.w3.org/2001/04/xmlenc#aes256-cbc", license->Crypto()->ContentKeyAlgorithm().c_str());
        ASSERT_STREQ("Enter your passphrase", license->Crypto()->UserKeyHint().c_str());
        ASSERT_STREQ("http://www.w3.org/2001/04/xmlenc#sha256", license->Crypto()->UserKeyAlgorithm().c_str());
        ASSERT_STREQ("XW3MtvGZrWe74uhsrPRVki0eSDTTtM2x+g6YIA8BSObfWEeaLRRkoxGOgRuEcsnT2Fphhs5rB2xW8NI+mnHlIg==", license->Crypto()->UserKeyCheck().c_str());
        ASSERT_STREQ(TestSignature.c_str(), license->Crypto()->Signature().c_str());
        ASSERT_STREQ("http://www.w3.org/2001/04/xmldsig-more#rsa-sha256", license->Crypto()->SignatureAlgorithm().c_str());
        ASSERT_STREQ(TestCertificate.c_str(), license->Crypto()->SignatureCertificate().c_str());

        ASSERT_TRUE(license->Links()->Has(lcp::Hint));
        lcp::Link hintLink;
        ASSERT_TRUE(license->Links()->GetLink(lcp::Hint, hintLink));
        ASSERT_STREQ("http://example.com/hint", hintLink.href.c_str());

        ASSERT_TRUE(license->Links()->Has(lcp::Publication));
        lcp::Link pubLink;
        ASSERT_TRUE(license->Links()->GetLink(lcp::Publication, pubLink));
        ASSERT_STREQ("http://www.neovento.net:8989/files/c82b4df43ef55e563fe57d45bf887a27425cf7c0967c16679ece11e74ec48ff7", pubLink.href.c_str());
        ASSERT_STREQ("application/epub+zip", pubLink.type.c_str());

        ASSERT_TRUE(license->User()->HasUserValue("id"));
        ASSERT_STREQ("mmenu", license->User()->Id().c_str());

        ASSERT_TRUE(license->Rights()->HasRightValue("tts"));
        ASSERT_TRUE(license->Rights()->HasRightValue("edit"));

        std::string ttsRightVal;
        ASSERT_TRUE(license->Rights()->GetRightValue(lcp::TtsRight, ttsRightVal));
        ASSERT_STREQ("true", ttsRightVal.c_str());

        std::string editRightVal;
        ASSERT_TRUE(license->Rights()->GetRightValue("edit", editRightVal));
        ASSERT_STREQ("false", editRightVal.c_str());
    }

    TEST_F(LcpServiceTest, LcpServiceOpen_DecryptLicense)
    {
        std::fstream mobyDickLicenseFile("..\\..\\..\\test\\lcp-client-lib\\data\\moby-dick-20120118.epub\\META-INF\\license.lcpl");
        std::string mobyDickLicenseStr(
            (std::istreambuf_iterator<char>(mobyDickLicenseFile)),
            std::istreambuf_iterator<char>()
            );

        lcp::ILicense * license = nullptr;
        lcp::Status res = m_lcpService->OpenLicense(mobyDickLicenseStr, &license);
        ASSERT_EQ(lcp::StatusCode::ErrorCommonSuccess, res.Code);

        res = m_lcpService->DecryptLicense(license, "White whales are huge!");
        ASSERT_EQ(lcp::StatusCode::ErrorCommonSuccess, res.Code);
    }
}