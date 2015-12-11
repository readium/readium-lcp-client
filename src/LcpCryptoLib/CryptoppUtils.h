//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#ifndef __CERTIFICATE_UTILS_H__
#define __CERTIFICATE_UTILS_H__

#include <string>
#include <cryptopp/rsa.h>
#include <cryptopp/secblock.h>
#include "LcpTypedefs.h"

using namespace CryptoPP;

namespace lcp
{
    class CryptoppUtils
    {
    public:
        static void Base64ToSecBlock(const std::string & base64, SecByteBlock & result);
        static Buffer Base64ToVector(const std::string & base64);
        static std::string RawToHex(const Buffer & key);
        static Buffer HexToRaw(const std::string & hex);
        static std::string GenerateUuid();

        class Cert
        {
        public:
            static std::string IntegerToString(const Integer & integer);
            static void SkipNextSequence(BERSequenceDecoder & parentSequence);

            static std::string ReadIntegerAsString(BERSequenceDecoder & sequence);
            static word32 ReadVersion(BERSequenceDecoder & toBeSignedCertificate, word32 defaultVersion);
            static void ReadOID(BERSequenceDecoder & certificate, OID & algorithmId);
            static void ReadSubjectPublicKey(BERSequenceDecoder & toBeSignedCertificate, RSA::PublicKey & result);
            static void ReadDateTimeSequence(
                BERSequenceDecoder & toBeSignedCertificate,
                std::string & notBefore,
                std::string & notAfter
                );

            static void PullToBeSignedData(const SecByteBlock & rawCertificate, SecByteBlock & result);

            static const byte ContextSpecificTagZero  = 0xa0;
            static const byte ContextSpecificTagThree = 0xa3;
            static const byte ContextSpecificTagSixIA5String = 0x86;
        };
    };
}

#endif //__CERTIFICATE_UTILS_H__
