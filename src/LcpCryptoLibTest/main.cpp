//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#include <iostream>
#include <memory>
#include <fstream>
#include "Public/lcp.h"
#include "TestStorageProvider.h"
#include "TestNetProvider.h"
#include "TestAcquisitionCallback.h"

int main(int argc, char ** argv)
{
    try
    {
        const char jsonLicenseSpec[] =
            u8"{ \"id\": \"ef15e740-697f-11e3-949a-0800200c9a66\","
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
                "\"end\" : \"2016-11-25T01:08:15+01:00\","
                "\"http://www.imaginaryebookretailer.com/lcp/rights/tweet\" : true,"
                "\"http://www.imaginaryebookretailer.com/lcp/rights/tweet234\" : 2.4894324894e-5,"
                "\"http://www.imaginaryebookretailer.com/lcp/rights/tweet234546\" : 0e-5,"
                "\"http://www.imaginaryebookretailer.com/lcp/rights/tweet2345463453\" : \"Hello world!\""
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

        std::string rootCertificate = "MIIDRjCCAq+gAwIBAgIJAMfRkXzjGB0rMA0GCSqGSIb3DQEBBQUAMHYxCzAJBgNV"
            "BAYTAkZSMQ8wDQYDVQQIDAZGcmFuY2UxDjAMBgNVBAcMBVBhcmlzMRAwDgYDVQQK"
            "DAdNYW50YW5vMRAwDgYDVQQDDAdNYW50YW5vMSIwIAYJKoZIhvcNAQkBFhNzdXBw"
            "b3J0QG1hbnRhbm8uY29tMB4XDTE1MTEyNDEyMjM1N1oXDTQ1MTExNjEyMjM1N1ow"
            "djELMAkGA1UEBhMCRlIxDzANBgNVBAgMBkZyYW5jZTEOMAwGA1UEBwwFUGFyaXMx"
            "EDAOBgNVBAoMB01hbnRhbm8xEDAOBgNVBAMMB01hbnRhbm8xIjAgBgkqhkiG9w0B"
            "CQEWE3N1cHBvcnRAbWFudGFuby5jb20wgZ8wDQYJKoZIhvcNAQEBBQADgY0AMIGJ"
            "AoGBAMI6xYGeIyijZ7/ha0GBmhe1VkUGtYiNPS4EqAJux57931cRCg6M8DkgyEnM"
            "Mu8hJBI+HCuFqPd3+IPsWt3aIChsJ2e2Th1ZmhEMGcO0m4JaXmONwj18dS8v9aZt"
            "OM1C+TP7UxV3VepYHM7ahYvtPA0yeC/ddb/pEveg/4O077EvAgMBAAGjgdswgdgw"
            "DAYDVR0TBAUwAwEB/zAdBgNVHQ4EFgQUKPgrvZlHIFg5aLtfhIeqeDa5cHkwgagG"
            "A1UdIwSBoDCBnYAUKPgrvZlHIFg5aLtfhIeqeDa5cHmheqR4MHYxCzAJBgNVBAYT"
            "AkZSMQ8wDQYDVQQIDAZGcmFuY2UxDjAMBgNVBAcMBVBhcmlzMRAwDgYDVQQKDAdN"
            "YW50YW5vMRAwDgYDVQQDDAdNYW50YW5vMSIwIAYJKoZIhvcNAQkBFhNzdXBwb3J0"
            "QG1hbnRhbm8uY29tggkAx9GRfOMYHSswDQYJKoZIhvcNAQEFBQADgYEAOFax2o1B"
            "zmY2V4GQYTr41QlOkTyOk2UEtyBtCwsGKXH02H+Vu7wEqBKMVL9+QQTtqCSFlEPA"
            "hoU8QCRfW2yPKRjiw2TeEHGJWV/QHcL74yTDNZZW3OHfF2tyiTNnzu4dX5k09Q8i"
            "gNBrawNJbGxYeRSVi6/AqZ8tX1g61G0SJ9w=";

        TestNetProvider netProvider;
        TestStorageProvider storageProvider("..\\..\\..\\src\\testing-data\\storage.json");
        lcp::DefaultFileSystemProvider fsProvider;

        lcp::ILcpService * rawSvcPtr = nullptr;
        lcp::LcpServiceCreator creator;
        lcp::Status res = creator.CreateLcpService(rootCertificate, &netProvider, &storageProvider, &fsProvider, &rawSvcPtr);
        std::unique_ptr<lcp::ILcpService> lcpService(rawSvcPtr);
        if (!lcp::Status::IsSuccess(res))
        {
            std::cout << "Status: " << res.ResultCode << "; Extension: " << res.Extension << std::endl;
            std::cin.get();
            return 0;
        }

        std::fstream mobyDickLicenseFile("..\\..\\..\\src\\testing-data\\moby-dick-20120118.epub\\META-INF\\license.lcpl");
        std::string mobyDickLicenseStr(
            (std::istreambuf_iterator<char>(mobyDickLicenseFile)),
            std::istreambuf_iterator<char>()
            );

        lcp::ILicense * rawLicPtr = nullptr;
        res = lcpService->OpenLicense(mobyDickLicenseStr, &rawLicPtr);
        //res = lcpService->OpenLicense(jsonLicenseSpec, &rawLicPtr);
        if (!lcp::Status::IsSuccess(res))
        {
            std::cout << "Status: " << res.ResultCode << "; Extension: " << res.Extension << std::endl;
            std::cin.get();
            return 0;
        }

        std::cout << rawLicPtr->CanonicalContent() << std::endl;
        std::cout << "License parsed successfully!" << std::endl;

        if (!rawLicPtr->Decrypted())
        {
            res = lcpService->DecryptLicense(rawLicPtr, "White whales are huge!");
            if (!lcp::Status::IsSuccess(res))
            {
                std::cout << "Status: " << res.ResultCode << "; Extension: " << res.Extension << std::endl;
                std::cin.get();
                return 0;
            }
            std::cout << "License decrypted successfully!" << std::endl;
        }
        else
        {
            std::cout << "License decrypted successfully by user key from the storage!" << std::endl;
        }

        // One-shot decryption
        std::fstream encryptedFile("..\\..\\..\\src\\testing-data\\moby-dick-20120118.epub\\OPS\\chapter_001.xhtml", std::ios::in | std::ios::binary);
        std::string encryptedFileStr(
            (std::istreambuf_iterator<char>(encryptedFile)),
            std::istreambuf_iterator<char>()
            );

        size_t outSize = 0;
        std::vector<unsigned char> decrypted(encryptedFileStr.size());
        res = lcpService->DecryptData(
            rawLicPtr,
            reinterpret_cast<const unsigned char *>(encryptedFileStr.data()),
            encryptedFileStr.size(),
            decrypted.data(),
            decrypted.size(),
            &outSize,
            "http://www.w3.org/2001/04/xmlenc#aes256-cbc"
            );
        if (!lcp::Status::IsSuccess(res))
        {
            std::cout << "Status: " << res.ResultCode << "; Extension: " << res.Extension << std::endl;
            std::cin.get();
            return 0;
        }
        decrypted.resize(outSize);
        encryptedFile.close();

        //Ranged decryption
        std::unique_ptr<lcp::IFile> file(fsProvider.GetFile(
            "..\\..\\..\\src\\testing-data\\moby-dick-20120118.epub\\OPS\\chapter_001.xhtml",
            lcp::IFileSystemProvider::ReadOnly)
            );

        lcp::IEncryptedStream * rawEncryptedStream = nullptr;
        res = lcpService->CreateEncryptedDataStream(
            rawLicPtr,
            file.get(),
            "http://www.w3.org/2001/04/xmlenc#aes256-cbc",
            &rawEncryptedStream
            );
        std::unique_ptr<lcp::IEncryptedStream> encryptedStream(rawEncryptedStream);
        if (!lcp::Status::IsSuccess(res))
        {
            std::cout << "Status: " << res.ResultCode << "; Extension: " << res.Extension << std::endl;
            std::cin.get();
            return 0;
        }

        std::vector<unsigned char> decryptedStart(32);
        encryptedStream->SetReadPosition(0);
        encryptedStream->Read(decryptedStart.data(), decryptedStart.size());

        std::vector<unsigned char> decryptedMiddleRange(32);
        encryptedStream->SetReadPosition(32);
        encryptedStream->Read(decryptedMiddleRange.data(), decryptedMiddleRange.size());

        std::vector<unsigned char> decryptedMiddleRange2(46);
        encryptedStream->SetReadPosition(78);
        encryptedStream->Read(decryptedMiddleRange2.data(), decryptedMiddleRange2.size());

        std::vector<unsigned char> decryptedLastRange(32);
        encryptedStream->SetReadPosition(encryptedStream->DecryptedSize() - 32);
        encryptedStream->Read(decryptedLastRange.data(), decryptedLastRange.size());


        lcp::IAcquisition * rawAcqPtr = nullptr;
        res = lcpService->CreatePublicationAcquisition("..\\..\\..\\src\\testing-data\\result.epub", rawLicPtr, &rawAcqPtr);
        std::unique_ptr<lcp::IAcquisition> acquisition(rawAcqPtr);
        if (!lcp::Status::IsSuccess(res))
        {
            std::cout << "Status: " << res.ResultCode << "; Extension: " << res.Extension << std::endl;
            std::cin.get();
            return 0;
        }
        TestAcquisitionCallback callback;
        res = acquisition->Start(&callback);
        if (!lcp::Status::IsSuccess(res))
        {
            std::cout << "Status: " << res.ResultCode << "; Extension: " << res.Extension << std::endl;
            std::cin.get();
            return 0;
        }

        lcp::IRightsService * service = lcpService->GetRightsService();
        bool boolRes = service->CanUseRight(rawLicPtr, lcp::PrintRight);
        boolRes = service->CanUseRight(rawLicPtr, lcp::CopyRight);
        boolRes = service->CanUseRight(rawLicPtr, lcp::TtsRight);

        boolRes = service->UseRight(rawLicPtr, lcp::PrintRight, 200);
        boolRes = service->UseRight(rawLicPtr, lcp::CopyRight, 500);
        std::string newCopyVal = service->GetValue(rawLicPtr, lcp::CopyRight);

        service->SetValue(rawLicPtr, lcp::PrintRight, "200000");
        service->SetValue(rawLicPtr, "http://www.righttorewrite/com", "true");

        std::cin.get();

        storageProvider.Flush();
    }
    catch (const std::exception & ex)
    {
        std::cerr << ex.what() << std::endl;
    }
    
    return 0;
}