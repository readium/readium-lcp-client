#include <iostream>
#include <memory>

#include "LcpPublicInterfaces.h"
#include "LcpServiceCreator.h"

int main()
{
    try
    {
        const char jsonLicense[] =
            "{ \"id\": \"ef15e740-697f-11e3-949a-0800200c9a66\","
            "\"issued\": \"2013-11-04T01:08:15+01:00\","
            "\"updated\": \"2014-02-21T09:44:17+01:00\","
            "\"provider\": \"http://www.imaginaryebookretailer.com\","
            "\"encryption\": {"
                "\"profile\": \"http://readium.org/lcp/profile-1.0\","
                "\"content_key\": {"
                    "\"encrypted_value\": \"/k8RpXqf4E2WEunCp76E8PjhS051NXwAXeTD1ioazYxCRGvHLAck/KQ3cCh5JxDmCK0nRLyAxs1X0aA3z55boQ==\","
                    "\"algorithm\": \"http://www.w3.org/2001/04/xmlenc#aes256-cbc\""
                "},"
                "\"user_key\": {"
                    "\"text_hint\": \"Enter your email address\","
                    "\"algorithm\": \"http://www.w3.org/2001/04/xmlenc#sha256\","
                    "\"key_check\": \"jJEjUDipHK3OjGt6kFq7dcOLZuicQFUYwQ+TYkAIWKm6Xv6kpHFhF7LOkUK/Owww\""
                "}"
            "},"
            "\"links\": {"
                "\"publication\": {"
                    "\"href\": \"http://www.example.com/file.epub\","
                    "\"type\" : \"application/epub+zip\","
                    "\"length\" : \"\","
                    "\"hash\" : \"\""
                "},"
                "\"hint\": {"
                    "\"href\": \"http://www.example.com/passphraseHint?user_id=1234\","
                    "\"type\" : \"text/html\""
                "},"
                 "\"http://mylcpextension.com/authentication\" : {"
                        "\"href\": \"http://www.example.com/authenticateMe\","
                        "\"title\" : \"Authentication service\","
                        "\"type\" : \"application/vnd.myextension.authentication+json\""
                "},"
                "\"http://mylcpextension.com/book_recommendations\" : ["
                        "{"
                            "\"href\": \"http://www.example.com/recommended/1\","
                            "\"type\" : \"text/html\""
                        "},"
                        "{"
                            "\"href\": \"http://www.example.com/recommended/1.opds\","
                            "\"type\" : \"application/atom+xml; profile=opds-catalog; kind=acquisition\""
                        "}"
                        "]"
            "},"
            "\"rights\": {"
                "\"print\": 0,"
                "\"copy\" : 2048,"
                "\"tts\" : true,"
                "\"start\" : \"2013-11-04T01:08:15+01:00\","
                "\"end\" : \"2013-11-25T01:08:15+01:00\","
                "\"http://www.imaginaryebookretailer.com/lcp/rights/tweet\" : true"
            "},"
            "\"user\": {"
            "\"id\": \"d9f298a7-7f34-49e7-8aae-4378ecb1d597\","
            "\"email\" : \"EnCt2b8c6d2afd94ae4ed201b27049d8ce1afe31a90ceb8c6d2afd94ae4ed201b2704RjkaXRveAAarHwdlID1KCIwEmS\","
            "\"encrypted\" : [\"email\"],"
            "\"http://www.imaginaryebookretailer.com/lcp/user/language\" : \"tlh\""
            "},"
            "\"signature\": {"
                "\"algorithm\": \"http://www.w3.org/2001/04/xmldsig-more#rsa-sha256\","
                "\"certificate\" : \"MIIDEjCCAfoCCQDwMOjkYYOjPjANBgkqhkiG9w0BAQUFADBLMQswCQYDVQQGEwJVUzETMBEGA1UECBMKQ2FsaWZ"
                "vcm5pYTETMBEGA1UEBxMKRXZlcnl3aGVyZTESMBAGA1UEAxMJbG9jYWxob3N0MB4XDTE0MDEwMjIxMjYxNloXDTE1MDEwMjIxMjYxNlowS"
                "zELMAkGA1UEBhMCVVMxEzARBgNVBAgTCkNhbGlmb3JuaWExEzARBgNVBAcTCkV2ZXJ5d2hlcmUxEjAQBgNVBAMTCWxvY2FsaG9zdDCCASI"
                "wDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAOpCRECG7icpf0H37kuAM7s42oqggBoikoTpo5yapy+s5eFSp8HSqwhIYgZ4SghNLkj3e"
                "652SALav7chyZ2vWvitZycY+aq50n5UTTxDvdwsC5ZNeTycuzVWZALKGhV7VUPEhtWZNm0gruntronNa8l2WS0aF7P5SbhJ65SDQGprFFa"
                "YOSyN6550P3kqaAO7tDddcA1cmuIIDRf8tOIIeMkBFk1Qf+lh+3uRP2wztOTECSMRxX/hIkCe5DRFDK2MuDUyc/iY8IbY0hMFFGw5J7MWO"
                "wZLBOaZHX+Lf5lOYByPbMH78O0dda6T+tLYAVzsmJdHJFtaRguCaJVtSXKQUAMCAwEAATANBgkqhkiG9w0BAQUFAAOCAQEAi9HIM+FMfqX"
                "sRUY0rGxLlw403f3YtAG/ohzt5i8DKiKUG3YAnwRbL/VzXLZaHru7XBC40wmKefKqoA0RHyNEddXgtY/aXzOlfTvp+xirop+D4DwJIbaj8"
                "/wHKWYGBucA/VgGY7JeSYYTUSuz2RoYtjPNRELIXN8A+D+nkJ3dxdFQ6jFfVfahN3nCIgRqRIOt1KaNI39CShccCaWJ5DeSASLXLPcEjrTi"
                "/pyDzC4kLF0VjHYlKT7lq5RkMO6GeC+7YFvJtAyssM2nqunA2lUgyQHb1q4Ih/dcYOACubtBwW0ITpHz8N7eO+r1dtH/BF4yxeWl6p5kGLvuPXNU21ThgA==\","
                "\"value\" : \"q/3IInic9c/EaJHyG1Kkqk5v1zlJNsiQBmxz4lykhyD3dA2jg2ZzrOenYU9GxP/xhe5H5Kt2WaJ/hnt8+GWrEx1QOwnNE"
                "ij5CmIpZ63yRNKnFS5rSRnDMYmQT/fkUYco7BUi7MPPU6OFf4+kaToNWl8m/ZlMxDcS3BZnVhSEKzUNQn1f2y3sUcXjes7wHbImDc6dRthbL"
                "/E+assh5HEqakrDuA4lM8XNfukEYQJnivqhqMLOGM33RnS5nZKrPPK/c2F/vGjJffSrlX3W3Jlds0/MZ6wtVeKIugR06c56V6+qKsnMLAQJa"
                "eOxxBXmbFdAEyplP9irn4D9tQZKqbbMIw==\""
            "}"
        "}";

        lcp::LcpServiceCreator creator;
        lcp::ILcpService * rawSvcPtr = nullptr;
        lcp::Status res = creator.CreateLcpService(&rawSvcPtr);
        std::unique_ptr<lcp::ILcpService> lcpService(rawSvcPtr);
        if (lcp::Status::IsSuccess(res))
        {
            lcp::ILicense * rawLicPtr = nullptr;
            res = lcpService->OpenLicense(jsonLicense, &rawLicPtr);
            std::unique_ptr<lcp::ILicense> license(rawLicPtr);
            if (lcp::Status::IsSuccess(res))
            {
                std::cout << "License parsed successfully!" << std::endl;
            }
            else
            {
                std::string jsonLicenseStr = jsonLicense;
                std::string substr = jsonLicenseStr.substr(614, std::string::npos);
                std::cout << "Status: " << res.ResultCode << "; Extension: " << res.Extension << std::endl;
            }
        }
        else
        {
            std::cout << "Status: " << res.ResultCode << "; Extension: " << res.Extension << std::endl;
        }
    }
    catch (const std::exception & ex)
    {
        std::cerr << ex.what() << std::endl;
    }

    std::cin.get();
    return 0;
}