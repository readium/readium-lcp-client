//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#include "CryptoppUtils.h"
#include "IncludeMacros.h"
#include <sstream>

CRYPTOPP_INCLUDE_START
#include <cryptopp/base64.h>
#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>
CRYPTOPP_INCLUDE_END

using namespace CryptoPP;

namespace lcp
{
    word32 CryptoppUtils::Cert::Cert::ReadVersion(BERSequenceDecoder & toBeSignedCertificate, word32 defaultVersion)
    {
        word32 version = defaultVersion;
        byte versionTag = toBeSignedCertificate.PeekByte();
        if (versionTag == ContextSpecificTagZero)
        {
            BERGeneralDecoder context(toBeSignedCertificate, ContextSpecificTagZero);
            BERDecodeUnsigned<word32>(toBeSignedCertificate, version);
        }
        else if (versionTag == INTEGER)
        {
            BERDecodeUnsigned<word32>(toBeSignedCertificate, version);
        }
        return version;
    }

    void CryptoppUtils::Cert::SkipNextSequence(BERSequenceDecoder & parentSequence)
    {
        BERSequenceDecoder sequence(parentSequence);
        sequence.SkipAll();
    }

    std::string CryptoppUtils::Cert::IntegerToString(const Integer & integer)
    {
        std::stringstream strm;
        strm << integer;
        std::string result(strm.str());
        return result.substr(0, result.find_first_of('.'));
    }

    std::string CryptoppUtils::Cert::ReadIntegerAsString(BERSequenceDecoder & sequence)
    {
        Integer value;
        value.BERDecode(sequence);
        return CryptoppUtils::Cert::Cert::IntegerToString(value);
    }

    std::vector<unsigned char> CryptoppUtils::Base64ToVector(const std::string & base64)
    {
        if (base64.empty())
        {
            throw std::runtime_error("base64 data is empty");
        }

        Base64Decoder decoder;
        decoder.Put(reinterpret_cast<const byte *>(base64.data()), base64.size());
        decoder.MessageEnd();

        std::vector<unsigned char> result;
        lword size = decoder.MaxRetrievable();
        if (size > 0 && size <= SIZE_MAX)
        {
            result.resize(static_cast<size_t>(size));
            decoder.Get(reinterpret_cast<byte *>(result.data()), result.size());
        }
        else
        {
            throw std::runtime_error("result data is empty");
        }
        return result;
    }

    std::string CryptoppUtils::GenerateUuid()
    {
        const static int UuidRawSize = 16;

        AutoSeededRandomPool rnd;
        std::vector<unsigned char> guid(UuidRawSize);
        rnd.GenerateBlock(guid.data(), guid.size());

        std::stringstream guidHex;
        guidHex << RawToHex(guid);
        return guidHex.str().insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-');
    }

    std::string CryptoppUtils::RawToHex(const std::vector<unsigned char> & key)
    {
        std::string hex;
        CryptoPP::ArraySource hexSource(
            &key.at(0),
            key.size(),
            true,
            new CryptoPP::HexEncoder(
                new CryptoPP::StringSink(hex),
                false
                )
            );
        return hex;
    }

    std::vector<unsigned char> CryptoppUtils::HexToRaw(const std::string & hex)
    {
        std::vector<unsigned char> value(hex.size() / 2);
        CryptoPP::StringSource(
            hex, true,
            new CryptoPP::HexDecoder(
                new CryptoPP::ArraySink(
                    &value.at(0), value.size()
                    )
                )
            );
        return value;
    }

    void CryptoppUtils::Base64ToSecBlock(const std::string & base64, SecByteBlock & result)
    {
        if (base64.empty())
        {
            throw std::runtime_error("base64 data is empty");
        }

        Base64Decoder decoder;
        decoder.Put(reinterpret_cast<const byte *>(base64.data()), base64.size());
        decoder.MessageEnd();

        lword size = decoder.MaxRetrievable();
        if (size > 0 && size <= SIZE_MAX)
        {
            result.resize(static_cast<size_t>(size));
            decoder.Get(reinterpret_cast<byte *>(result.data()), result.size());
        }
        else
        {
            throw std::runtime_error("result data is empty");
        }
    }

    void CryptoppUtils::Cert::ReadDateTimeSequence(
        BERSequenceDecoder & toBeSignedCertificate,
        std::string & notBefore,
        std::string & notAfter
        )
    {
        BERSequenceDecoder validity(toBeSignedCertificate);
        {
            BERDecodeTime(validity, notBefore);
            BERDecodeTime(validity, notAfter);
        }
        validity.MessageEnd();
    }

    void CryptoppUtils::Cert::ReadSubjectPublicKey(BERSequenceDecoder & toBeSignedCertificate, RSA::PublicKey & result)
    {
        ByteQueue subjectPublicKey;

        BERSequenceDecoder subjPublicInfoFrom(toBeSignedCertificate);
        DERSequenceEncoder subjPublicInfoOut(subjectPublicKey);
        subjPublicInfoFrom.TransferTo(subjPublicInfoOut, subjPublicInfoFrom.RemainingLength());
        subjPublicInfoOut.MessageEnd();
        subjPublicInfoFrom.MessageEnd();

        result.BERDecode(subjectPublicKey);
    }

    void CryptoppUtils::Cert::ReadOID(BERSequenceDecoder & certificate, OID & algorithmId)
    {
        BERSequenceDecoder algorithm(certificate);
        algorithmId.BERDecode(algorithm);
        algorithm.SkipAll();
    }

    void CryptoppUtils::Cert::PullToBeSignedData(const SecByteBlock & rawCertificate, SecByteBlock & result)
    {
        if (rawCertificate.empty())
        {
            throw std::runtime_error("raw certificate data is empty");
        }

        ByteQueue certificateQueue;
        ByteQueue resultQueue;
        certificateQueue.Put(rawCertificate.data(), rawCertificate.size());
        certificateQueue.MessageEnd();

        BERSequenceDecoder certificate(certificateQueue);
        BERSequenceDecoder tbsCertificate(certificate);
        DERSequenceEncoder tbsCertifciateEnc(resultQueue);
        tbsCertificate.CopyTo(tbsCertifciateEnc);
        tbsCertifciateEnc.MessageEnd();

        lword size = resultQueue.MaxRetrievable();
        if (size > 0 && size <= SIZE_MAX)
        {
            result.resize(static_cast<size_t>(size));
            resultQueue.Get(result.data(), result.size());
        }
        else
        {
            throw std::runtime_error("signed data is empty");
        }
    }
}