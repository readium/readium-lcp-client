#ifndef __CERTIFICATE_H__
#define __CERTIFICATE_H__

#include <string>
#include <cryptopp/rsa.h>
#include <cryptopp/secblock.h>
#include "Public/LcpStatus.h"

namespace lcp
{
    class Certificate
    {
    public:
        Certificate(const std::string & certificateBase64);

        std::string SerialNumber() const;
        std::string NotBeforeDate() const;
        std::string NotAfterDate() const;
        std::vector<unsigned char> PublicKey() const;

        bool ValidateCertificate(Certificate * rootCertificate);
        bool ValidateMessage(const std::string & message, const std::string & hashBase64);

    private:
        std::string m_serialNumber;
        std::string m_notBeforeDate;
        std::string m_notAfterDate;
        CryptoPP::RSA::PublicKey m_publicKey;
        CryptoPP::SecByteBlock m_toBeSignedSequence;
        CryptoPP::SecByteBlock m_rootSignature;
        CryptoPP::OID m_signatureAlgorithm;
    };
}

#endif //__CERTIFICATE_H__
