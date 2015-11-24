#include <iostream>
#include <memory>
#include <map>
#include <fstream>

#include "Public/lcp.h"

class StorageProvider : public lcp::IStorageProvider
{
    typedef std::map<std::string, std::string> StringsMap;

public:
    virtual void SetValue(const std::string & vaultId, const std::string & key, const std::string & value)
    {
        StringsMap * vaultPtr = this->FindVault(vaultId);
        auto res = vaultPtr->insert(std::make_pair(key, value));
        if (!res.second)
        {
            throw std::runtime_error("Duplicate record");
        }
    }

    virtual std::string GetValue(const std::string & vaultId, const std::string & key)
    {
        StringsMap * vaultPtr = this->FindVault(vaultId);
        auto res = vaultPtr->find(key);
        if (res != vaultPtr->end())
            return res->second;
        return std::string();
    }

    virtual lcp::MapIterator<std::string> * EnumerateVault(const std::string & vaultId)
    {
        StringsMap * vaultPtr = this->FindVault(vaultId);
        return new lcp::MapIterator<std::string>(*vaultPtr);
    }

private:
    StringsMap * FindVault(const std::string & vaultId)
    {
        StringsMap * mapPtr = nullptr;
        if (vaultId == lcp::UserKeysVaultId)
        {
            mapPtr = &m_userKeysVault;
        }
        else if (vaultId == lcp::LicenseRightsVaultId)
        {
            mapPtr = &m_licienseRightsVault;
        }
        else
        {
            throw std::runtime_error("Vault was not found");
        }
        return mapPtr;
    }

private:
    StringsMap m_userKeysVault;
    StringsMap m_licienseRightsVault;
};

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
                "\"end\" : \"2013-11-25T01:08:15+01:00\","
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

        std::fstream mobyDickLicenseFile("c:\\Users\\Main\\Documents\\BitBucket\\mantano-lcp-client\\src\\LcpCryptoLibTest\\license.lcpl");
        std::string mobyDickLicenseStr(
            (std::istreambuf_iterator<char>(mobyDickLicenseFile)),
            std::istreambuf_iterator<char>()
            );

        std::string testbase64Cert = "MIIC1jCCAb6gAwIBAgIQKDb7Oazo77RIbUBWHaRaWzANBgkqhkiG9w0BAQUFADAU"
            "MRIwEAYDVQQDEwlsb2NhbGhvc3QwHhcNMTUwNzMwMTQyMDMwWhcNMjAwNzMwMDAw"
            "MDAwWjAUMRIwEAYDVQQDEwlsb2NhbGhvc3QwggEiMA0GCSqGSIb3DQEBAQUAA4IB"
            "DwAwggEKAoIBAQC7uvRfxl0aJQLD929XkAY0C/keu5Rx2Z1MyQr7RdZh2AS1lmdl"
            "QASJt4TSchkosWE5LrAGoRyonR7fYS38+xNN2kV0yTQhMjOBddqHuZEvgzKkJiDK"
            "A4d355RgY/mp2vNlxaQpHNSMTq73OmuwSLYHsB3IhcRnnSa6ZOZ4zh0fMp+Pw9L1"
            "ZX0PjhKElXHRdMIfjNBXs7OUk5tjBrpZIqL0zcUaLfTnaG85WTzzhYshtVzdg8Wy"
            "/pU1ckNvOiY9m9sE77VwIZpQ7ENKxLkPuFhVnzpJEVv0FGOe1CaYWlY7xOTGJsYI"
            "glBALtczRciq5SK8n9APt2pL7EJTCCr18PCPAgMBAAGjJDAiMAsGA1UdDwQEAwIE"
            "sDATBgNVHSUEDDAKBggrBgEFBQcDATANBgkqhkiG9w0BAQUFAAOCAQEAMSmWkvN5"
            "RLfyHPpCIZm98Io42R2W5ti52bJcH9sRqLL+N1YyLtVzxnlkjT6F8Q9zo30F0o7J"
            "L29DBRXdi3LX8Px5qX4qRrmGKJRXbP4QV9IP4j12z7ZWh+YASWW5+lwlAYA2wsX1"
            "doBC7iwDCbEAWAPY7hMfjGEvMTP3oObEPMbTWcwmia2W+brB2cgKy6mnhUZP9KLe"
            "vf42VGLQyKzvbVFcrg0nk33r/85Pw+/eWUya3QIGHZj86j9amyZl1APuUT7997cj"
            "Lii1sC5fv3GHHUGAfoHKkosCGJhmXU6VP7y7sdOQ0weiZCHtuikYmNvoLs/jr1rE"
            "91yNXO9v5kh/0w==";

        std::string rootCertificate = "MIICOzCCAaQCCQDc/yCIGe/zZjANBgkqhkiG9w0BAQUFADBiMQswCQYDVQQGEwJG"
            "UjEPMA0GA1UECBMGRnJhbmNlMQ4wDAYDVQQHEwVQYXJpczEQMA4GA1UEChMHTWFu"
            "dGFubzEgMB4GCSqGSIb3DQEJARYRbW1lbnVAbWFudGFuby5jb20wHhcNMTUxMTIz"
            "MTcwMDM0WhcNMTYxMTIyMTcwMDM0WjBiMQswCQYDVQQGEwJGUjEPMA0GA1UECBMG"
            "RnJhbmNlMQ4wDAYDVQQHEwVQYXJpczEQMA4GA1UEChMHTWFudGFubzEgMB4GCSqG"
            "SIb3DQEJARYRbW1lbnVAbWFudGFuby5jb20wgZ8wDQYJKoZIhvcNAQEBBQADgY0A"
            "MIGJAoGBALqiIMlTO8ZCOk9HVudSIcnnkztYmnRrt95s6saqErNHlIxEuHFVQqnB"
            "aK166xAgKqXdRi8hK7sDapT2SolWg4y4nNo7sUdbs+woRIjV1fuoZCqMMCrplgRL"
            "oB12b4J4bifkw9hFTpyq+0k6zDzejHPl6+OfIEEuN9oUIiX7Uvm1AgMBAAEwDQYJ"
            "KoZIhvcNAQEFBQADgYEAeZQWt5ZUCM6FqH1bkb7KGVkKaw4WABv0MPb57hc4JqCA"
            "du1u5AqRJ6Ama5WPxsOQhQ6tAEinUI6KTnnn/Hr5mFIOYKiVZb8kUll5Q5dLPXY4"
            "mRVA/lWpVs01xsDQFHIQpI9KJk4XsA85qrvFhFl9SSbZGWzgMcEvf0Yw9GLuLYU=";

        lcp::INetProvider * netProvider = nullptr;
        std::unique_ptr<lcp::IStorageProvider> storageProvider(new StorageProvider());

        lcp::ILcpService * rawSvcPtr = nullptr;
        lcp::LcpServiceCreator creator;
        lcp::Status res = creator.CreateLcpService(rootCertificate, netProvider, storageProvider.get(), &rawSvcPtr);
        std::unique_ptr<lcp::ILcpService> lcpService(rawSvcPtr);
        if (lcp::Status::IsSuccess(res))
        {
            lcp::ILicense * rawLicPtr = nullptr;
            res = lcpService->OpenLicense(mobyDickLicenseStr, &rawLicPtr);
            //res = lcpService->OpenLicense(jsonLicenseSpec, &rawLicPtr);
            std::unique_ptr<lcp::ILicense> license(rawLicPtr);
            if (lcp::Status::IsSuccess(res) || res.ResultCode == lcp::StCodeCover::ErrorOpeningLicenseStillEncrypted)
            {
                std::cout << license->Content() << std::endl;
                std::cout << "License parsed successfully!" << std::endl;

                if (!license->Decrypted())
                {
                    res = lcpService->DecryptLicense(license.get(), "White whales are huge!");
                    if (!lcp::Status::IsSuccess(res))
                    {
                        std::cout << res.Extension << std::endl;
                    }
                }
            }
            else
            {
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