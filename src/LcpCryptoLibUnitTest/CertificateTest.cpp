#include <gtest/gtest.h>
#include "TestUtils.h"

#include <cryptopp/cryptlib.h>
using CryptoPP::PrivateKey;
using CryptoPP::PublicKey;

#include <cryptopp/queue.h>
using CryptoPP::ByteQueue;

#include <cryptopp/rsa.h>
using CryptoPP::RSA;
using CryptoPP::RSASS;
using CryptoPP::PKCS1v15;

#include <cryptopp/osrng.h>
using CryptoPP::AutoSeededRandomPool;

#include <cryptopp/base64.h>
using CryptoPP::Base64Decoder;
using CryptoPP::Base64Encoder;

#include <cryptopp/sha.h>
using CryptoPP::SHA256;

#include <cryptopp/hex.h>
using CryptoPP::HexEncoder;

#include <cryptopp/filters.h>
using CryptoPP::ArraySource;
using CryptoPP::StringSink;

#include <cryptopp/files.h>
using CryptoPP::FileSource;
using CryptoPP::FileSink;

#include <cryptopp/asn.h>
using CryptoPP::BERSequenceDecoder;
using CryptoPP::DERSequenceEncoder;
using CryptoPP::BERGeneralDecoder;
using CryptoPP::Integer;

using CryptoPP::SecByteBlock;
using CryptoPP::lword;
using CryptoPP::word32;

#include "Certificate.h"
#include "Public/lcp.h"


namespace lcptest
{

    typedef RSASS<PKCS1v15, SHA256>::Signer RSASS_PKCS_SHA256_Signer;
    typedef RSASS<PKCS1v15, SHA256>::Verifier RSASS_PKCS_SHA256_Verifier;

    TEST(Certificate, VeryifySignature)
    {
        try
        {
            const char jsonLicense[] =
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

            lcp::LcpServiceCreator creator;
            lcp::ILcpService * rawSvcPtr = nullptr;
            std::string rootCertificate;
            lcp::Status res = creator.CreateLcpService(rootCertificate, &rawSvcPtr);
            std::unique_ptr<lcp::ILcpService> lcpService(rawSvcPtr);
            if (lcp::Status::IsSuccess(res))
            {
                lcp::ILicense * rawLicPtr = nullptr;
                res = lcpService->OpenLicense(jsonLicense, &rawLicPtr);
                if (lcp::Status::IsSuccess(res))
                {
                    std::cout << rawLicPtr->Content() << std::endl;
                    std::cout << "License parsed successfully!" << std::endl;
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

        std::string canonicalJson = u8"{\"encryption\":{\"content_key\":{\"algorithm\":\"http://www.w3.org/2001/04/xmlenc#aes256-cbc\","
            "\"encrypted_value\":\"/k8RpXqf4E2WEunCp76E8PjhS051NXwAXeTD1ioazYxCRGvHLAck/KQ3cCh5JxDmCK0nRLyAxs1X0aA3z55boQ==\"},\"profile\""
            ":\"http://readium.org/lcp/profile-1.0\",\"user_key\":{\"algorithm\":\"http://www.w3.org/2001/04/xmlenc#sha256\",\"key_check\""
            ":\"jJEjUDipHK3OjGt6kFq7dcOLZuicQFUYwQ+TYkAIWKm6Xv6kpHFhF7LOkUK/Owww\",\"text_hint\":\"Enter your email address\"}},\"id\""
            ":\"ef15e740-697f-11e3-949a-0800200c9a66\",\"issued\":\"2013-11-04T01:08:15+01:00\",\"links\":{\"hint\":{\"href\""
            ":\"http://www.imaginaryebookretailer.com/lcp/hint\",\"type\":\"text/html\"}},\"provider\":\"http://www.imaginaryebookretailer.com\","
            "\"updated\":\"2014-02-21T09:44:17+01:00\",\"user\":{\"id\":\"d9f298a7-7f34-49e7-8aae-4378ecb1d597\"}}";


        AutoSeededRandomPool rnd;
        RSA::PrivateKey rsaPrivate;
        rsaPrivate.GenerateRandomWithKeySize(rnd, 2048);
        RSA::PublicKey publicKey(rsaPrivate);

        RSASS_PKCS_SHA256_Signer theSigner(rsaPrivate);
        size_t maxSignSize = theSigner.MaxSignatureLength();
        SecByteBlock rawSign(maxSignSize);
        size_t realSignSize = theSigner.SignMessage(rnd, (byte*)canonicalJson.data(), canonicalJson.size(), rawSign);
        rawSign.resize(realSignSize);


        std::string base64Sign;
        {
            Base64Encoder encoder;
            encoder.Put(rawSign.BytePtr(), rawSign.SizeInBytes());
            encoder.MessageEnd();
            lword size = encoder.MaxRetrievable();
            if (size > 0 && size <= SIZE_MAX)
            {
                base64Sign.resize(size);
                encoder.Get((byte *)base64Sign.data(), base64Sign.size());
            }
        }

        std::string base64Cert;
        ByteQueue certQueue;
        SecByteBlock rawCert;
        //publicKey.DEREncodePublicKey(certQueue);
        publicKey.DEREncode(certQueue);
        {
            lword size = certQueue.MaxRetrievable();
            if (size > 0 && size <= SIZE_MAX)
            {
                rawCert.resize(size);
                certQueue.Get(rawCert.data(), rawCert.size());

                Base64Encoder encoder;
                encoder.Put(rawCert.data(), rawCert.size());
                encoder.MessageEnd();
                lword base64Size = encoder.MaxRetrievable();
                if (base64Size > 0 && base64Size <= SIZE_MAX)
                {
                    base64Cert.resize(base64Size);
                    encoder.Get((byte *)base64Cert.data(), base64Cert.size());
                }
            }
        }

        //lcp::Certificate cert(base64Cert);
        /*cert.ValidateMessage((unsigned char *)canonicalJson.data(), canonicalJson.size(),
            rawSign.data(), rawSign.size());*/

        SecByteBlock rawDecodedSign;
        {
            Base64Decoder decoderSign;
            decoderSign.Put((byte *)base64Sign.data(), base64Sign.size());
            decoderSign.MessageEnd();

            lword size = decoderSign.MaxRetrievable();
            if (size > 0 && size <= SIZE_MAX)
            {
                rawDecodedSign.resize(size);
                decoderSign.Get((byte *)rawDecodedSign.data(), rawDecodedSign.size());
            }
        }

        ASSERT_TRUE(ArraysMatch<SecByteBlock>(rawSign, rawDecodedSign));


        // lcp cert
        //std::string testbase64Cert = "MIIDEjCCAfoCCQDwMOjkYYOjPjANBgkqhkiG9w0BAQUFADBLMQswCQYDVQQGEwJVUzETMBEGA1UECBMKQ2FsaWZvcm5pYTETMBEGA1UEBxMKRXZlcnl3aGVyZTESMBAGA1UEAxMJbG9jYWxob3N0MB4XDTE0MDEwMjIxMjYxNloXDTE1MDEwMjIxMjYxNlowSzELMAkGA1UEBhMCVVMxEzARBgNVBAgTCkNhbGlmb3JuaWExEzARBgNVBAcTCkV2ZXJ5d2hlcmUxEjAQBgNVBAMTCWxvY2FsaG9zdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAOpCRECG7icpf0H37kuAM7s42oqggBoikoTpo5yapy+s5eFSp8HSqwhIYgZ4SghNLkj3e652SALav7chyZ2vWvitZycY+aq50n5UTTxDvdwsC5ZNeTycuzVWZALKGhV7VUPEhtWZNm0gruntronNa8l2WS0aF7P5SbhJ65SDQGprFFaYOSyN6550P3kqaAO7tDddcA1cmuIIDRf8tOIIeMkBFk1Qf+lh+3uRP2wztOTECSMRxX/hIkCe5DRFDK2MuDUyc/iY8IbY0hMFFGw5J7MWOwZLBOaZHX+Lf5lOYByPbMH78O0dda6T+tLYAVzsmJdHJFtaRguCaJVtSXKQUAMCAwEAATANBgkqhkiG9w0BAQUFAAOCAQEAi9HIM+FMfqXsRUY0rGxLlw403f3YtAG/ohzt5i8DKiKUG3YAnwRbL/VzXLZaHru7XBC40wmKefKqoA0RHyNEddXgtY/aXzOlfTvp+xirop+D4DwJIbaj8/wHKWYGBucA/VgGY7JeSYYTUSuz2RoYtjPNRELIXN8A+D+nkJ3dxdFQ6jFfVfahN3nCIgRqRIOt1KaNI39CShccCaWJ5DeSASLXLPcEjrTi/pyDzC4kLF0VjHYlKT7lq5RkMO6GeC+7YFvJtAyssM2nqunA2lUgyQHb1q4Ih/dcYOACubtBwW0ITpHz8N7eO+r1dtH/BF4yxeWl6p5kGLvuPXNU21ThgA==";
        // ss.com online gen
        /*std::string testbase64Cert = "MIIC/TCCAeWgAwIBAgIJAJLEh2Aa2YxXMA0GCSqGSIb3DQEBBQUAMBUxEzARBgNV"
            "BAMMCnd3dy5zcy5jb20wHhcNMTUxMTE2MDAxODA1WhcNMjUxMTEzMDAxODA1WjAV"
            "MRMwEQYDVQQDDAp3d3cuc3MuY29tMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIB"
            "CgKCAQEAsVmlGwn1FsIvJIunadrnxCPnH8/BClckac4wPy2RKCb+dhIso4WbSHoQ"
            "qJql2BOm4GyEpYbRgnhQYca9sfbdU3n6aCm+TIDLOPLYfjLSwk84aCGsansc7QDb"
            "YaUciktjVIp330DcJ0jHBoM3d2b5V/ldebp3z4uEZAscU6fRusb9qtOcBArz7xiz"
            "ek1m/fgWKAgVEkAtpEFxVqFXjqs3doRlIYP6IIWbPDPaKXYVmeCUNrMzcltdX1sQ"
            "/BpMWzDzDb/OMk0inejVY9vU16GFOkvRQ0vbPUUSVX995plFD6LaAzd16k9DogOj"
            "5pAYQiGoR2/HwzlosWfBwBSoriN2XwIDAQABo1AwTjAdBgNVHQ4EFgQUSCshmgKI"
            "zoGC/hDbLb25TN+QtIAwHwYDVR0jBBgwFoAUSCshmgKIzoGC/hDbLb25TN+QtIAw"
            "DAYDVR0TBAUwAwEB/zANBgkqhkiG9w0BAQUFAAOCAQEAGlc5qh9n8wlCtsRNb7tP"
            "3oHpUBFANlXXxuA1LtZDWMXzIkdE0vVFjPMnbOAtFEL8Hgu2y+GSHFDYnmcKKdlw"
            "+4LjTzROTiHUHlSwgf/vU0sdexJCuhUrEtlPe9PJ+hWx8zdVMrS0rD4+NAm+ViEr"
            "O+7XogSPE7r4J+uL4d53K5xNGynEMcICiPzw2zk/cQFU8qFvcKOR9ngedcbkFrAM"
            "TQt13/Y3BrjQRrC62bHfu8bmqvaX/XAeTl+RzQGqW5th9MaENAPUd2I1djniW4iK"
            "xX9g8wbHWzzOdVxDgG1oguG1R7WwpZ+Lil8mMrcLwI2Q9kOWAP/MWCGJuJuToT1k"
            "dw==";*/
        // localhost
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

        SecByteBlock rawDecodedCert;
        {
            Base64Decoder decoderCert;
            decoderCert.Put((byte *)base64Cert.data(), base64Cert.size());
            decoderCert.MessageEnd();

            lword size = decoderCert.MaxRetrievable();
            if (size > 0 && size <= SIZE_MAX)
            {
                rawDecodedCert.resize(size);
                decoderCert.Get((byte *)rawDecodedCert.data(), rawDecodedCert.size());
            }
        }

        ASSERT_TRUE(ArraysMatch<SecByteBlock>(rawCert, rawDecodedCert));

        ByteQueue publicKeyQueue;
        publicKeyQueue.Put(rawDecodedCert.data(), rawDecodedCert.size());
        publicKeyQueue.MessageEnd();

        RSA::PublicKey decodedPublicKey;
        //decodedPublicKey.BERDecodePublicKey(publicKeyQueue, false, publicKeyQueue.MaxRetrievable());
        decodedPublicKey.BERDecode(publicKeyQueue);

        RSASS_PKCS_SHA256_Verifier verifier(decodedPublicKey);

        bool res = verifier.VerifyMessage(
            (byte *)canonicalJson.data(), canonicalJson.size(),
            rawDecodedSign.data(), rawDecodedSign.size()
            );

        ASSERT_TRUE(res);
    }
}