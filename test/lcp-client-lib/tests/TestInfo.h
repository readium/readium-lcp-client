//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#ifndef __TEST_INFO_H__
#define __TEST_INFO_H__

#include <string>

namespace lcptest
{
    static const unsigned char TestUserKey[] =
    { 0xb5, 0xcd, 0x12, 0x60, 0xcd, 0x3d, 0xbd, 0xd2, 0x9a, 0x57, 0x87, 0x3f, 0xfd, 0x2d, 0xdd, 0xd6,
        0x4c, 0x79, 0xc4, 0x0e, 0x9e, 0xaf, 0x34, 0x25, 0x42, 0x3d, 0x6a, 0xaf, 0x19, 0xd3, 0x13, 0x85 };

    static const unsigned char TestContentKey[] =
    { 0xe5, 0x42, 0xdb, 0x50, 0xa5, 0xa8, 0x89, 0x68, 0x4f, 0xa8, 0x14, 0x01, 0xc0, 0x42, 0x80, 0x3a,
        0xd7, 0x4a, 0xf6, 0xfe, 0x3f, 0x27, 0xfb, 0x11, 0x1b, 0x30, 0x0a, 0x44, 0x34, 0xf8, 0xb2, 0x5d };

    static std::string TestCanonicalJson = u8"{\"encryption\":{\"content_key\":{\"algorithm\":\"http://www.w3.org/2001/04/xmlenc#aes256-cbc\","
        "\"encrypted_value\":\"7GMxrWNzUFpcwRNhclNECE3ZxvODsBt13N52zBoWdd6lQSNITVEJBXKxencovnPBBthBgE/6UE3sEmZYSfMrkQ==\"},\"profile\""
        ":\"http://readium.org/lcp/profile-1.0\",\"user_key\":{\"algorithm\":\"http://www.w3.org/2001/04/xmlenc#sha256\",\"key_check\":"
        "\"XW3MtvGZrWe74uhsrPRVki0eSDTTtM2x+g6YIA8BSObfWEeaLRRkoxGOgRuEcsnT2Fphhs5rB2xW8NI+mnHlIg==\",\"text_hint\":\"Enter your "
        "passphrase\"}},\"id\":\"df09ac25-a386-4c5c-b167-33ce4c36ca65\",\"issued\":\"2015-11-24T13:32:39.905384033+01:00\",\"links\""
        ":{\"hint\":{\"href\":\"http://example.com/hint\"},\"publication\":{\"href\":"
        "\"http://www.neovento.net:8989/files/c82b4df43ef55e563fe57d45bf887a27425cf7c0967c16679ece11e74ec48ff7\",\"type\":"
        "\"application/epub+zip\"}},\"provider\":\"http://example.com\",\"rights\":{\"edit\":false,\"tts\":true},\"updated\""
        ":\"2015-11-24T13:32:39.905384033+01:00\",\"user\":{\"id\":\"mmenu\"}}";

    static std::string TestSignature = u8"YIQsNYo+MC25Zl2XVS1cObuYvFkYTyRoLA9v/A/2bax/G9lYxVHwUTawLD0QWNSmlyzW2EZH84GgbV9"
        "tDIpQxbFzRwXznuB4AAWflvnYIcUAT5CLnJ3fcTtvSFxjIT9rLMyxxYAUdbqhFR0S9fYNcuQ2f8bKussOH6c5WUZwMK8=";

    static std::string TestRootCertificate = u8"MIIDRjCCAq+gAwIBAgIJAMfRkXzjGB0rMA0GCSqGSIb3DQEBBQUAMHYxCzAJBgNV"
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

    static std::string TestCertificate = u8"MIIDRjCCAq+gAwIBAgIJAMfRkXzjGB0rMA0GCSqGSIb3DQEBBQUAMHYxCzAJBgNV"
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

    static std::string TestDistributionPointCert = u8"MIIC8jCCAdqgAwIBAgIDEjRWMA0GCSqGSIb3DQEBCwUAMCExCzAJBgNVBAYTAlVB"
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

    static std::string TestCrl = "MIIDFDCCAfwCAQEwDQYJKoZIhvcNAQEFBQAwXzEjMCEGA1UEChMaU2FtcGxlIFNp"
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

    static std::string TestSsDotComCertificate = "MIIC/TCCAeWgAwIBAgIJAJLEh2Aa2YxXMA0GCSqGSIb3DQEBBQUAMBUxEzARBgNV"
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
        "dw==";
}

#endif //__TEST_INFO_H__