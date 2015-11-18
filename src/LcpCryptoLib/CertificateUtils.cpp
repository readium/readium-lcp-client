#include <sstream>
#include <cryptopp/base64.h>
#include "CertificateUtils.h"

using namespace CryptoPP;

namespace lcp
{
    word32 CertificateUtils::ReadVersion(BERSequenceDecoder & toBeSignedCertificate)
    {
        try
        {
            BERGeneralDecoder context(toBeSignedCertificate, ContextSpecificTag);
            word32 version = 0;
            BERDecodeUnsigned<word32>(toBeSignedCertificate, version);
            return version;
        }
        catch (const BERDecodeErr & ex)
        {
            ex;
            // If the version field is absent - default is v1
            return 0;
        }
    }

    void CertificateUtils::SkipNextSequence(BERSequenceDecoder & parentSequence)
    {
        BERSequenceDecoder sequence(parentSequence);
        sequence.SkipAll();
    }

    std::string CertificateUtils::IntegerToString(const Integer & integer)
    {
        std::stringstream strm;
        strm << integer;
        std::string result(strm.str());
        return result.substr(0, result.find_first_of('.'));
    }

    std::string CertificateUtils::ReadIntegerAsString(BERSequenceDecoder & sequence)
    {
        Integer value;
        value.BERDecode(sequence);
        return CertificateUtils::IntegerToString(value);
    }

    void CertificateUtils::Base64ToSecBlock(const std::string & base64, SecByteBlock & result)
    {
        Base64Decoder decoder;
        decoder.Put((byte *)base64.data(), base64.size());
        decoder.MessageEnd();

        lword size = decoder.MaxRetrievable();
        if (size > 0 && size <= SIZE_MAX)
        {
            result.resize(size);
            decoder.Get((byte *)result.data(), result.size());
        }
    }

    void CertificateUtils::ReadDateTimeSequence(
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

    void CertificateUtils::ReadSubjectPublicKey(BERSequenceDecoder & toBeSignedCertificate, RSA::PublicKey & result)
    {
        ByteQueue subjectPublicKey;

        BERSequenceDecoder subjPublicInfoFrom(toBeSignedCertificate);
        DERSequenceEncoder subjPublicInfoOut(subjectPublicKey);
        subjPublicInfoFrom.CopyTo(subjPublicInfoOut);
        subjPublicInfoOut.MessageEnd();

        result.BERDecode(subjectPublicKey);
    }

    void CertificateUtils::ReadOID(BERSequenceDecoder & certificate, OID & algorithmId)
    {
        BERSequenceDecoder algorithm(certificate);
        algorithmId.BERDecode(algorithm);
        algorithm.SkipAll();
    }

    void CertificateUtils::PullToBeSignedData(const SecByteBlock & rawCertificate, SecByteBlock & result)
    {
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
            result.resize(size);
            resultQueue.Get(result.data(), result.size());
        }
    }
}