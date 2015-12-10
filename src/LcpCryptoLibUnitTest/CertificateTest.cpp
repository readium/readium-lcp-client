//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

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

#include <cryptopp/hex.h>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>

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
#include "CertificateRevocationList.h"
#include "EncryptionProfilesManager.h"
#include "Public/lcp.h"

namespace lcptest
{

    typedef RSASS<PKCS1v15, SHA256>::Signer RSASS_PKCS_SHA256_Signer;
    typedef RSASS<PKCS1v15, SHA256>::Verifier RSASS_PKCS_SHA256_Verifier;

    TEST(Certificate, VeryifySignature)
    {
        std::string distributionPointCert = "MIIC8jCCAdqgAwIBAgIDEjRWMA0GCSqGSIb3DQEBCwUAMCExCzAJBgNVBAYTAlVB"
            "MRIwEAYDVQQKEwlNeUNvbXBhbnkwHhcNMTUxMjA2MTg0NDAwWhcNMTYxMjA2MTg0"
            "NDAwWjAhMQswCQYDVQQGEwJVQTESMBAGA1UEChMJTXlDb21wYW55MIIBIjANBgkq"
            "hkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA1lTzjJ/lgUPhpuhAz9Da2dRb5LrFDsl9"
            "a11R5KOiw51LkVnbhhuBNFWHFVlwzbyxLQsg3FGAQuCP4q1U5FWip9b8Rv6RRSKJ"
            "bpLeBfyRiPPswiSrLBViXd9bYZh0DjoUIudoFnAE11TjyNVgNEsC6wJDfYRFaj5H"
            "AVEu9WrV3paaoEjAijB87JaUJXoiIO0/kig9PJC2j4fkBABrPZbnanFT90xVzcsw"
            "AlTAMIhvYcNrQzzXFn3uWFdUQOWaJcY5V6NC9g7o6rl2yWIqFZzCuo4M9A1nQvmg"
            "JYPA9B7muUYwbgGmCy1GkmVNKBp2rPaG9bFVHuiKMU2PxbX5JbbkqwIDAQABozMw"
            "MTAvBgNVHR8EKDAmMCSgIqAghh5odHRwOi8vbG9jYWxob3N0OjgwODAvdGVzdC5j"
            "cmwwDQYJKoZIhvcNAQELBQADggEBAKzW3Z9c89LbLyby4zRA5GdCbJdB0bNGwpEm"
            "doqPB6DXck0lQnC7yMqTJOcl5GoAfZ4EVuKuxb+PUs7WU3gxq/8Mg5Ov6CVZ0Pqi"
            "y3ajpq3Tgq9asyocK+xalwgFNfBp1SuRbVgXwxKCpSXyrM41Z7nncuQ3Vwm7GmrU"
            "xR+qA5CMyTtLz88U2PxYHR/dAp7PTPhQzpbUV+KZhr1KbTkn4CZdHjDO7RjakRow"
            "QqiNwdeywh1cWKLj4aCmXbBv4I+Ptm21Sff+Wy30MNmxZsCFPD0mICSubBl7SByT"
            "dNHfZQ+wnGnvqTaNtVeDmJPi98lAwCNt7iNiBy0veVrMcwAsHB4=";

        std::string testCrl = "MIIDFDCCAfwCAQEwDQYJKoZIhvcNAQEFBQAwXzEjMCEGA1UEChMaU2FtcGxlIFNp"
            "Z25lciBPcmdhbml6YXRpb24xGzAZBgNVBAsTElNhbXBsZSBTaWduZXIgVW5pdDEb"
            "MBkGA1UEAxMSU2FtcGxlIFNpZ25lciBDZXJ0Fw0xMzAyMTgxMDMyMDBaFw0xMzAy"
            "MTgxMDQyMDBaMIIBNjA8AgMUeUcXDTEzMDIxODEwMjIxMlowJjAKBgNVHRUEAwoB"
            "AzAYBgNVHRgEERgPMjAxMzAyMTgxMDIyMDBaMDwCAxR5SBcNMTMwMjE4MTAyMjIy"
            "WjAmMAoGA1UdFQQDCgEGMBgGA1UdGAQRGA8yMDEzMDIxODEwMjIwMFowPAIDFHlJ"
            "Fw0xMzAyMTgxMDIyMzJaMCYwCgYDVR0VBAMKAQQwGAYDVR0YBBEYDzIwMTMwMjE4"
            "MTAyMjAwWjA8AgMUeUoXDTEzMDIxODEwMjI0MlowJjAKBgNVHRUEAwoBATAYBgNV"
            "HRgEERgPMjAxMzAyMTgxMDIyMDBaMDwCAxR5SxcNMTMwMjE4MTAyMjUxWjAmMAoG"
            "A1UdFQQDCgEFMBgGA1UdGAQRGA8yMDEzMDIxODEwMjIwMFqgLzAtMB8GA1UdIwQY"
            "MBaAFL4SAcyq6hGA2i6tsurHtfuf+a00MAoGA1UdFAQDAgEDMA0GCSqGSIb3DQEB"
            "BQUAA4IBAQBCIb6B8cN5dmZbziETimiotDy+FsOvS93LeDWSkNjXTG/+bGgnrm3a"
            "QpgB7heT8L2o7s2QtjX2DaTOSYL3nZ/Ibn/R8S0g+EbNQxdk5/la6CERxiRp+E2T"
            "UG8LDb14YVMhRGKvCguSIyUG0MwGW6waqVtd6K71u7vhIU/Tidf6ZSdsTMhpPPFu"
            "PUid4j29U3q10SGFF6cCt1DzjvUcCwHGhHA02Men70EgZFADPLWmLg0HglKUh1iZ"
            "WcBGtev/8VsUijyjsM072C6Ut5TwNyrrthb952+eKlmxLNgT0o5hVYxjXhtwLQsL"
            "7QZhrypAM1DLYqQjkiDI7hlvt7QuDGTJ";


        lcp::EncryptionProfilesManager profilesManager;
        lcp::IEncryptionProfile * profile = profilesManager.GetProfile("http://readium.org/lcp/profile-1.0");
        if (profile == nullptr)
        {
            throw std::runtime_error("error");
        }

        std::unique_ptr<lcp::Certificate> rootCertificate;
        try
        {
            rootCertificate.reset(new lcp::Certificate(distributionPointCert, profile));
        }
        catch (CryptoPP::BERDecodeErr & ex)
        {
            throw std::runtime_error(ex.GetWhat());
        }

        std::unique_ptr<lcp::Certificate> providerCertificate;
        try
        {
            providerCertificate.reset(new lcp::Certificate(distributionPointCert, profile));
        }
        catch (CryptoPP::BERDecodeErr & ex)
        {
            throw std::runtime_error(ex.GetWhat());
        }

        lcp::Buffer rawDecoded;
        {
            Base64Decoder decoder;
            decoder.Put((byte *)testCrl.data(), testCrl.size());
            decoder.MessageEnd();

            lword size = decoder.MaxRetrievable();
            if (size > 0 && size <= SIZE_MAX)
            {
                rawDecoded.resize(static_cast<size_t>(size));
                decoder.Get((byte *)rawDecoded.data(), rawDecoded.size());
            }
        }


        lcp::CertificateRevocationList revocation(rawDecoded);
        std::string thisUpdate = revocation.ThisUpdateDate();
        lcp::StringsSet serials = revocation.RevokedSerialNumbers();


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
                base64Sign.resize(static_cast<size_t>(size));
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
                rawCert.resize(static_cast<size_t>(size));
                certQueue.Get(rawCert.data(), rawCert.size());

                Base64Encoder encoder;
                encoder.Put(rawCert.data(), rawCert.size());
                encoder.MessageEnd();
                lword base64Size = encoder.MaxRetrievable();
                if (base64Size > 0 && base64Size <= SIZE_MAX)
                {
                    base64Cert.resize(static_cast<size_t>(base64Size));
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
                rawDecodedSign.resize(static_cast<size_t>(size));
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
                rawDecodedCert.resize(static_cast<size_t>(size));
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