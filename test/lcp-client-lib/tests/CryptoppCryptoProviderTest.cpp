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
#include "LcpUtils.h"
#include "CryptoppCryptoProvider.h"
#include "EncryptionProfilesManager.h"
#include "FakeLicenseImpl.h"

namespace lcptest
{
    class CryptoppCryptoProviderTest : public ::testing::Test
    {
    protected:
        void SetUp()
        {
            m_cryptoProvider.reset(new lcp::CryptoppCryptoProvider(&m_profilesManager, nullptr, ""));
        }
        void TearDown()
        {
            m_cryptoProvider.reset();
        }

    protected:
        lcp::EncryptionProfilesManager m_profilesManager;
        std::unique_ptr<lcp::CryptoppCryptoProvider> m_cryptoProvider;
    };

    TEST_F(CryptoppCryptoProviderTest, VerifyLicense_Return_Success)
    {
        FakeCryptoImpl crypto;
        FakeLicenseImpl license(&crypto);

        lcp::Status res = m_cryptoProvider->VerifyLicense(TestCertificate, &license);
        ASSERT_EQ(lcp::StatusCode::ErrorCommonSuccess, res.Code);
    }

    TEST_F(CryptoppCryptoProviderTest, VerifyLicense_Return_ErrorCommonEncryptionProfileNotFound)
    {
        FakeCryptoImpl crypto;
        crypto.SetEncryptionProfile("unknown");
        FakeLicenseImpl license(&crypto);

        lcp::Status res = m_cryptoProvider->VerifyLicense(TestCertificate, &license);
        ASSERT_EQ(lcp::StatusCode::ErrorCommonEncryptionProfileNotFound, res.Code);
    }

    TEST_F(CryptoppCryptoProviderTest, VerifyLicense_Return_ErrorOpeningNoRootCertificate)
    {
        FakeCryptoImpl crypto;
        FakeLicenseImpl license(&crypto);

        lcp::Status res = m_cryptoProvider->VerifyLicense("", &license);
        ASSERT_EQ(lcp::StatusCode::ErrorOpeningNoRootCertificate, res.Code);
    }

    TEST_F(CryptoppCryptoProviderTest, VerifyLicense_Return_ErrorOpeningRootCertificateNotValid)
    {
        FakeCryptoImpl crypto;
        FakeLicenseImpl license(&crypto);

        lcp::Status res = m_cryptoProvider->VerifyLicense("Invalid root certificate", &license);
        ASSERT_EQ(lcp::StatusCode::ErrorOpeningRootCertificateNotValid, res.Code);
    }

    TEST_F(CryptoppCryptoProviderTest, VerifyLicense_Return_ErrorOpeningContentProviderCertificateNotValid)
    {
        FakeCryptoImpl crypto;
        crypto.SetSignatureCertificate("Invalid provider certificate");
        FakeLicenseImpl license(&crypto);

        lcp::Status res = m_cryptoProvider->VerifyLicense(TestCertificate, &license);
        ASSERT_EQ(lcp::StatusCode::ErrorOpeningContentProviderCertificateNotValid, res.Code);
    }

    TEST_F(CryptoppCryptoProviderTest, VerifyLicense_Return_ErrorOpeningContentProviderCertificateNotValid_Exception)
    {
        FakeCryptoImpl crypto;
        crypto.SetSignatureCertificate(TestSsDotComCertificate);
        FakeLicenseImpl license(&crypto);

        try
        {
            lcp::Status res = m_cryptoProvider->VerifyLicense(TestCertificate, &license);
        }
        catch (const lcp::StatusException & ex)
        {
            ASSERT_EQ(lcp::StatusCode::ErrorOpeningContentProviderCertificateNotValid, ex.ResultStatus().Code);
        }
    }

    TEST_F(CryptoppCryptoProviderTest, VerifyLicense_Return_ErrorOpeningLicenseSignatureNotValid)
    {
        FakeCryptoImpl crypto;
        crypto.SetSignature(u8"YIQsNYo+MC25Xl2XVS1cObuYvFkYTyRoLA9v/A/2bax/G9lYxVHwUTawLD0QWNSmlyzW2EZH84GgbV9"
            "tDIpQxbFzRwXznuB4AAWflvnYIcUAT5CLnJ3fcTtvSFxjIT9rLMyxxYAUdbqhFR0S9fYNcuQ2f8bKussOH6c5WUZwMK8=");
        FakeLicenseImpl license(&crypto);

        lcp::Status res = m_cryptoProvider->VerifyLicense(TestCertificate, &license);
        ASSERT_EQ(lcp::StatusCode::ErrorOpeningLicenseSignatureNotValid, res.Code);
    }

    TEST_F(CryptoppCryptoProviderTest, VerifyLicense_Return_ErrorOpeningContentProviderCertificateNotStarted_UpdatedCheck)
    {
        FakeCryptoImpl crypto;
        FakeLicenseImpl license(&crypto);
        license.SetUpdated("2015-10-24T13:32:39+01:00");

        //not before: "2015-11-24T12:23:57Z"
        lcp::Status res = m_cryptoProvider->VerifyLicense(TestCertificate, &license);
        ASSERT_EQ(lcp::StatusCode::ErrorOpeningContentProviderCertificateNotStarted, res.Code);
    }

    TEST_F(CryptoppCryptoProviderTest, VerifyLicense_Return_ErrorOpeningContentProviderCertificateNotStarted_IssuedCheck)
    {
        FakeCryptoImpl crypto;
        FakeLicenseImpl license(&crypto);
        license.SetUpdated("");
        license.SetIssued("2015-10-24T13:32:39+01:00");

        //not before: "2015-11-24T12:23:57Z"
        lcp::Status res = m_cryptoProvider->VerifyLicense(TestCertificate, &license);
        ASSERT_EQ(lcp::StatusCode::ErrorOpeningContentProviderCertificateNotStarted, res.Code);
    }

    TEST_F(CryptoppCryptoProviderTest, VerifyLicense_Return_ErrorOpeningContentProviderCertificateExpired_UpdatedCheck)
    {
        FakeCryptoImpl crypto;
        FakeLicenseImpl license(&crypto);
        license.SetUpdated("2045-12-24T13:32:39+01:00");

        //not after: "2045-11-16T12:23:57Z"
        lcp::Status res = m_cryptoProvider->VerifyLicense(TestCertificate, &license);
        ASSERT_EQ(lcp::StatusCode::ErrorOpeningContentProviderCertificateExpired, res.Code);
    }

    TEST_F(CryptoppCryptoProviderTest, VerifyLicense_Return_ErrorOpeningContentProviderCertificateExpired_IssuedCheck)
    {
        FakeCryptoImpl crypto;
        FakeLicenseImpl license(&crypto);
        license.SetUpdated("");
        license.SetIssued("2045-12-24T13:32:39+01:00");

        //not after: "2045-11-16T12:23:57Z"
        lcp::Status res = m_cryptoProvider->VerifyLicense(TestCertificate, &license);
        ASSERT_EQ(lcp::StatusCode::ErrorOpeningContentProviderCertificateExpired, res.Code);
    }

    //////////////////////////////////////////////////////////////////////////

    TEST_F(CryptoppCryptoProviderTest, DecryptUserKey_Success)
    {
        FakeCryptoImpl crypto;
        FakeLicenseImpl license(&crypto);

        lcp::KeyType userKey;
        lcp::Status res = m_cryptoProvider->DecryptUserKey("White whales are huge!", &license, userKey);
        ASSERT_EQ(lcp::StatusCode::ErrorCommonSuccess, res.Code);
    }

    TEST_F(CryptoppCryptoProviderTest, DecryptUserKey_ErrorDecryptionUserPassphraseNotValid)
    {
        FakeCryptoImpl crypto;
        FakeLicenseImpl license(&crypto);

        lcp::KeyType userKey;
        lcp::Status res = m_cryptoProvider->DecryptUserKey("White whales are not huge!", &license, userKey);
        ASSERT_EQ(lcp::StatusCode::ErrorDecryptionUserPassphraseNotValid, res.Code);
    }

    //////////////////////////////////////////////////////////////////////////

    TEST_F(CryptoppCryptoProviderTest, DecryptContentKey_Success)
    {
        FakeCryptoImpl crypto;
        FakeLicenseImpl license(&crypto);

        lcp::KeyType userKey;
        lcp::Status res = m_cryptoProvider->DecryptUserKey("White whales are huge!", &license, userKey);
        ASSERT_EQ(lcp::StatusCode::ErrorCommonSuccess, res.Code);

        lcp::KeyType contentKey;
        res = m_cryptoProvider->DecryptContentKey(userKey, &license, contentKey);
        ASSERT_EQ(lcp::StatusCode::ErrorCommonSuccess, res.Code);
    }

    TEST_F(CryptoppCryptoProviderTest, DecryptContentKey_ErrorDecryptionLicenseEncrypted)
    {
        FakeCryptoImpl crypto;
        FakeLicenseImpl license(&crypto);

        std::string fakeKey("qwertyuiopasdfghjkl");
        lcp::KeyType userKey(fakeKey.begin(), fakeKey.end());

        lcp::KeyType contentKey;
        lcp::Status res = m_cryptoProvider->DecryptContentKey(userKey, &license, contentKey);
        ASSERT_EQ(lcp::StatusCode::ErrorDecryptionLicenseEncrypted, res.Code);
    }
}