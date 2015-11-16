#include "Certificate.h"
#include <sstream>
#include <memory>

#include <cryptopp/cryptlib.h>

#include <cryptopp/queue.h>
#include <cryptopp/base64.h>

#include <cryptopp/rsa.h>
#include <cryptopp/sha.h>
#include <cryptopp/md5.h>

#include <cryptopp/asn.h>

#include <cryptopp/oids.h>
using namespace CryptoPP;

// Signature Algorithm OIDs commonly used in certs that have RSA keys
DEFINE_OID(CryptoPP::ASN1::pkcs_1() + 4, md5withRSAEncryption);
DEFINE_OID(CryptoPP::ASN1::pkcs_1() + 5, sha1withRSAEncryption);
DEFINE_OID(CryptoPP::ASN1::pkcs_1() + 11, sha256withRSAEncryption);

namespace lcp
{
    Certificate::Certificate(const std::string & certificateBase64)
    {
        SecByteBlock rawDecodedCert;
        {
            Base64Decoder decoderCert;
            decoderCert.Put((byte *)certificateBase64.data(), certificateBase64.size());
            decoderCert.MessageEnd();

            lword size = decoderCert.MaxRetrievable();
            if (size > 0 && size <= SIZE_MAX)
            {
                rawDecodedCert.resize(size);
                decoderCert.Get((byte *)rawDecodedCert.data(), rawDecodedCert.size());
            }
        }

        ByteQueue certData;
        certData.Put(rawDecodedCert.data(), rawDecodedCert.size());
        certData.MessageEnd();

        ByteQueue tbsCertDataForHash;

        ByteQueue certDataForHash(certData); //copy of data
        BERSequenceDecoder certForHash(certDataForHash);
        BERSequenceDecoder tbsCertForHash(certForHash);
        DERSequenceEncoder tbsCertEncForHash(tbsCertDataForHash);
        tbsCertForHash.CopyTo(tbsCertEncForHash);
        tbsCertEncForHash.MessageEnd();

        BERSequenceDecoder cert(certData);
        {
            BERSequenceDecoder tbsCert(cert);
            {
                BERGeneralDecoder context(tbsCert, 0xa0);
                word32 ver = 0;
                CryptoPP::BERDecodeUnsigned<word32>(tbsCert, ver);

                Integer serial;
                serial.BERDecode(tbsCert);
                std::stringstream strm;
                strm << serial;
                m_serialNumber = strm.str();
                strm.clear();

                BERSequenceDecoder algorithmId(tbsCert);
                algorithmId.SkipAll();

                BERSequenceDecoder issuer(tbsCert);
                issuer.SkipAll();

                BERSequenceDecoder validity(tbsCert);
                {
                    SecByteBlock notBefore;
                    CryptoPP::BERDecodeTime(validity, notBefore);
                    m_notBeforeDate.assign(notBefore.begin(), notBefore.end());
                    SecByteBlock notAfter;
                    CryptoPP::BERDecodeTime(validity, notAfter);
                    m_notAfterDate.assign(notAfter.begin(), notAfter.end());
                }
                validity.MessageEnd();

                BERSequenceDecoder subject(tbsCert);
                subject.SkipAll();

                ByteQueue subjectPublicKey;
                BERSequenceDecoder subjPublicInfoFrom(tbsCert);
                DERSequenceEncoder subjPublicInfoOut(subjectPublicKey);
                subjPublicInfoFrom.CopyTo(subjPublicInfoOut);
                subjPublicInfoOut.MessageEnd();

                m_publicKey.BERDecode(subjectPublicKey);
            }
            tbsCert.SkipAll();

            BERSequenceDecoder signAlgorithm(cert);
            m_signatureAlgorithm.BERDecodeAndCheck(signAlgorithm);
            signAlgorithm.SkipAll();

            unsigned int unused = 0;
            BERDecodeBitString(cert, m_rootSignature, unused);
        }
    }

    std::vector<unsigned char> Certificate::PublicKey() const
    {
        ByteQueue publicKeyQueue;
        m_publicKey.DEREncode(publicKeyQueue);
        size_t size = publicKeyQueue.MaxRetrievable();
        std::vector<unsigned char> outKey(size);
        publicKeyQueue.Get(&outKey.at(0), outKey.size());
        return outKey;
    }

    bool Certificate::ValidateMessage(const std::string & message, const std::string & hashBase64)
    {
        SecByteBlock rawHash;
        {
            Base64Decoder decoder;
            decoder.Put((byte *)hashBase64.data(), hashBase64.size());
            decoder.MessageEnd();

            lword size = decoder.MaxRetrievable();
            if (size > 0 && size <= SIZE_MAX)
            {
                rawHash.resize(size);
                decoder.Get((byte *)hashBase64.data(), hashBase64.size());
            }
        }

        RSASS<PKCS1v15, SHA256>::Verifier verifier(m_publicKey);
        return verifier.VerifyMessage(
            (byte *)message.data(),
            message.size(),
            rawHash.data(),
            rawHash.size()
            );
    }

    bool Certificate::ValidateCertificate(Certificate * rootCertificate)
    {
        ByteQueue publicKeyQueue;
        std::vector<unsigned char> rawPublicKey = rootCertificate->PublicKey();
        publicKeyQueue.Put(&rawPublicKey.at(0), rawPublicKey.size());
        publicKeyQueue.MessageEnd();
        RSA::PublicKey publicKey;
        publicKey.BERDecode(publicKeyQueue);

        std::unique_ptr<PK_Verifier> rootVerifierPtr;

        if (m_signatureAlgorithm == sha256withRSAEncryption())
        {
            rootVerifierPtr.reset(new RSASS<PKCS1v15, SHA256>::Verifier(publicKey));
        }
        else if (m_signatureAlgorithm == sha1withRSAEncryption())
        {
            rootVerifierPtr.reset(new RSASS<PKCS1v15, SHA1>::Verifier(publicKey));
        }
        else if (m_signatureAlgorithm == md5withRSAEncryption())
        {
            rootVerifierPtr.reset(new RSASS<PKCS1v15, MD5>::Verifier(publicKey));
        }

        if (m_rootSignature.size() != rootVerifierPtr->SignatureLength())
        {
            ;//return status wrong hash function
        }

        return rootVerifierPtr->VerifyMessage(
            m_toBeSignedSequence.data(),
            m_toBeSignedSequence.size(),
            m_rootSignature.data(),
            m_rootSignature.size()
            );
    }

    std::string Certificate::SerialNumber() const
    {
        return m_serialNumber;
    }

    std::string Certificate::NotBeforeDate() const
    {
        return m_notBeforeDate;
    }

    std::string Certificate::NotAfterDate() const
    {
        return m_notAfterDate;
    }
}