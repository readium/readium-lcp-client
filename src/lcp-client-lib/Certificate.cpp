//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#include "Certificate.h"
#include "CryptoAlgorithmInterfaces.h"
#include "CryptoppUtils.h"
#include "IncludeMacros.h"
#include "IEncryptionProfile.h"
#include "LcpUtils.h"

CRYPTOPP_INCLUDE_START
#include <cryptopp/cryptlib.h>
#include <cryptopp/queue.h>
#include <cryptopp/rsa.h>
#include <cryptopp/sha.h>
#include <cryptopp/md5.h>
#include <cryptopp/asn.h>
#include <cryptopp/oids.h>
CRYPTOPP_INCLUDE_END

using namespace CryptoPP;

// Signature Algorithm OIDs commonly used in certs that have RSA keys
DEFINE_OID(ASN1::pkcs_1() + 4, md5withRSAEncryption);
DEFINE_OID(ASN1::pkcs_1() + 5, sha1withRSAEncryption);
DEFINE_OID(ASN1::pkcs_1() + 11, sha256withRSAEncryption);

DEFINE_OID(ASN1::joint_iso_ccitt() + 5, joint_iso_ccitt_ds);
DEFINE_OID(joint_iso_ccitt_ds() + 29, id_ce);
DEFINE_OID(id_ce() + 31, id_ce_CRLDistributionPoints);


namespace lcp
{
    Certificate::Certificate(
        const std::string & certificateBase64,
        IEncryptionProfile * encryptionProfile
        )
        : m_encryptionProfile(encryptionProfile)
    {
        SecByteBlock rawDecodedCert;
        CryptoppUtils::Base64ToSecBlock(certificateBase64, rawDecodedCert);

        ByteQueue certData;
        certData.Put(rawDecodedCert.data(), rawDecodedCert.size());
        certData.MessageEnd();

        BERSequenceDecoder cert(certData);
        {
            BERSequenceDecoder toBeSignedCert(cert);
            {
                word32 version = CryptoppUtils::Cert::ReadVersion(toBeSignedCert, CertificateVersion::Certificatev1);
                if (version != CertificateVersion::Certificatev3)
                {
                    throw BERDecodeErr("Wrong version of the certificate");
                }

                m_serialNumber = CryptoppUtils::Cert::ReadIntegerAsString(toBeSignedCert);

                // algorithmId
                CryptoppUtils::Cert::SkipNextSequence(toBeSignedCert);
                // issuer
                CryptoppUtils::Cert::SkipNextSequence(toBeSignedCert);
                // validity
                CryptoppUtils::Cert::ReadDateTimeSequence(toBeSignedCert, m_notBeforeDate, m_notAfterDate);

                // subject
                CryptoppUtils::Cert::SkipNextSequence(toBeSignedCert);

                CryptoppUtils::Cert::ReadSubjectPublicKey(toBeSignedCert, m_publicKey);


                while (!toBeSignedCert.EndReached())
                {
                    byte extensionsContext = toBeSignedCert.PeekByte();
                    if (extensionsContext != CryptoppUtils::Cert::ContextSpecificTagThree)
                    {
                        CryptoppUtils::Cert::SkipNextSequence(toBeSignedCert);//TODO: test
                        continue;
                    }

                    BERGeneralDecoder(toBeSignedCert, CryptoppUtils::Cert::ContextSpecificTagThree);
                    {
                        // Extensions
                        BERSequenceDecoder extensions(toBeSignedCert);
                        {
                            while (!extensions.EndReached())
                            {
                                CertificateExtension extension(extensions);
                                if (extension.CryptoOid() == id_ce_CRLDistributionPoints())
                                {
                                    m_distributionPoints.reset(new CrlDistributionPoints(&extension));
                                }
                                m_extensions.push_back(std::move(extension));
                            }
                        }
                        extensions.MessageEnd();
                    }
                }
            }
            toBeSignedCert.MessageEnd();

            CryptoppUtils::Cert::ReadOID(cert, m_signatureAlgorithmId);
            unsigned int unused = 0;
            BERDecodeBitString(cert, m_rootSignature, unused);
        }

        CryptoppUtils::Cert::PullToBeSignedData(rawDecodedCert, m_toBeSignedData);
        m_signatureAlgorithm.reset(m_encryptionProfile->CreateSignatureAlgorithm(this->PublicKey()));
    }

    KeyType Certificate::PublicKey() const
    {
        ByteQueue publicKeyQueue;
        m_publicKey.DEREncode(publicKeyQueue);
        size_t size = static_cast<size_t>(publicKeyQueue.MaxRetrievable());
        KeyType outKey(size);
        publicKeyQueue.Get(&outKey.at(0), outKey.size());
        return outKey;
    }

    bool Certificate::VerifyMessage(const std::string & message, const std::string & hashBase64)
    {
        return m_signatureAlgorithm->VerifySignature(message, hashBase64);
    }

    bool Certificate::VerifyMessage(
        const unsigned char * message,
        size_t messageLength,
        const unsigned char * signature,
        size_t signatureLength
        )
    {
        return m_signatureAlgorithm->VerifySignature(
            message,
            messageLength,
            signature,
            signatureLength
            );
    }

    bool Certificate::VerifyCertificate(ICertificate * rootCertificate)
    {
        ByteQueue publicKeyQueue;
        KeyType rawPublicKey = rootCertificate->PublicKey();
        publicKeyQueue.Put(&rawPublicKey.at(0), rawPublicKey.size());
        publicKeyQueue.MessageEnd();
        RSA::PublicKey publicKey;
        publicKey.BERDecode(publicKeyQueue);

        std::unique_ptr<PK_Verifier> rootVerifierPtr;

        if (m_signatureAlgorithmId == sha256withRSAEncryption())
        {
            rootVerifierPtr.reset(new RSASS<PKCS1v15, SHA256>::Verifier(publicKey));
        }
        else if (m_signatureAlgorithmId == sha1withRSAEncryption())
        {
            rootVerifierPtr.reset(new RSASS<PKCS1v15, SHA1>::Verifier(publicKey));
        }
        else if (m_signatureAlgorithmId == md5withRSAEncryption())
        {
            rootVerifierPtr.reset(new RSASS<PKCS1v15, Weak::MD5>::Verifier(publicKey));
        }
        else
        {
            throw StatusException(Status(StatusCode::ErrorOpeningRootCertificateSignatureAlgorithmNotFound));
        }

        if (m_rootSignature.size() != rootVerifierPtr->SignatureLength())
        {
            throw StatusException(Status(StatusCode::ErrorOpeningContentProviderCertificateNotValid));
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

    ICrlDistributionPoints * Certificate::DistributionPoints() const
    {
        return m_distributionPoints.get();
    }
}