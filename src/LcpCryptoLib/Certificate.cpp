#include <cryptopp/cryptlib.h>
#include <cryptopp/queue.h>

#include <cryptopp/rsa.h>
#include <cryptopp/sha.h>
#include <cryptopp/md5.h>

#include <cryptopp/asn.h>
#include <cryptopp/oids.h>

#include "Certificate.h"
#include "CertificateUtils.h"

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
        CertificateUtils::Base64ToSecBlock(certificateBase64, rawDecodedCert);

        ByteQueue certData;
        certData.Put(rawDecodedCert.data(), rawDecodedCert.size());
        certData.MessageEnd();

        BERSequenceDecoder cert(certData);
        {
            BERSequenceDecoder toBeSignedCert(cert);
            {
                word32 version = CertificateUtils::ReadVersion(toBeSignedCert);

                m_serialNumber = CertificateUtils::ReadIntegerAsString(toBeSignedCert);

                // algorithmId
                CertificateUtils::SkipNextSequence(toBeSignedCert);
                // issuer
                CertificateUtils::SkipNextSequence(toBeSignedCert);

                CertificateUtils::ReadDateTimeSequence(toBeSignedCert, m_notBeforeDate, m_notAfterDate);

                // subject
                CertificateUtils::SkipNextSequence(toBeSignedCert);

                CertificateUtils::ReadSubjectPublicKey(toBeSignedCert, m_publicKey);
            }
            toBeSignedCert.SkipAll();

            CertificateUtils::ReadOID(cert, m_signatureAlgorithm);

            unsigned int unused = 0;
            BERDecodeBitString(cert, m_rootSignature, unused);
        }

        CertificateUtils::PullToBeSignedData(rawDecodedCert, m_toBeSignedData);
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
        CertificateUtils::Base64ToSecBlock(hashBase64, rawHash);

        RSASS<PKCS1v15, SHA256>::Verifier verifier(m_publicKey);
        return verifier.VerifyMessage(
            (byte *)message.data(),
            message.size(),
            rawHash.data(),
            rawHash.size()
            );
    }

    bool Certificate::ValidateMessage(
        const unsigned char * message,
        size_t messageLength,
        const unsigned char *signature,
        size_t signatureLength
        )
    {
        RSASS<PKCS1v15, SHA256>::Verifier verifier(m_publicKey);
        return verifier.VerifyMessage(
            message,
            messageLength,
            signature,
            signatureLength
            );
    }

    bool Certificate::VerifyCertificate(ICertificate * rootCertificate)
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
            m_toBeSignedData.data(),
            m_toBeSignedData.size(),
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