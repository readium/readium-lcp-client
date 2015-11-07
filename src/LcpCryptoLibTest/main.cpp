#include <iostream>
#include <memory>

#include "LcpPublicInterfaces.h"
#include "LcpServiceCreator.h"

int main()
{
    try
    {
        const char jsonLicense[] = "{ \"id\": \"ef15e740-697f-11e3-949a-0800200c9a66\","
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
            "}"
            "}";

        lcp::LcpServiceCreator creator;
        lcp::ILcpService * rawSvcPtr = nullptr;
        lcp::Status res = creator.CreateLcpService(&rawSvcPtr);
        if (lcp::Status::IsSuccess(res))
        {
            std::unique_ptr<lcp::ILcpService> lcpService(rawSvcPtr);
            lcp::ILicense * rawLicPtr = nullptr;
            res = lcpService->OpenLicense(jsonLicense, &rawLicPtr);
            if (lcp::Status::IsSuccess(res))
            {
                std::unique_ptr<lcp::ILicense> license(rawLicPtr);
                std::cout << "License parsed successfully!" << std::endl;
            }
        }
    }
    catch (const std::exception & ex)
    {
        std::cerr << ex.what() << std::endl;
    }

    std::cin.get();
    return 0;
}