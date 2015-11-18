#ifndef __CERTIFICATE_H__
#define __CERTIFICATE_H__

#include <string>
#include <cryptopp/rsa.h>
#include <cryptopp/secblock.h>
#include "ICertificate.h"

using namespace CryptoPP;

namespace lcp
{
    class Certificate : public ICertificate
    {
    public:
        explicit Certificate(const std::string & certificateBase64);

        std::string SerialNumber() const;
        std::string NotBeforeDate() const;
        std::string NotAfterDate() const;
        std::vector<unsigned char> PublicKey() const;

        bool VerifyCertificate(ICertificate * rootCertificate);
        bool ValidateMessage(const std::string & message, const std::string & hashBase64);
        bool ValidateMessage(
            const unsigned char * message,
            size_t messageLength,
            const unsigned char *signature,
            size_t signatureLength
            );

    private:
        std::string m_serialNumber;
        std::string m_notBeforeDate;
        std::string m_notAfterDate;
        RSA::PublicKey m_publicKey;
        SecByteBlock m_toBeSignedData;
        SecByteBlock m_rootSignature;
        OID m_signatureAlgorithm;
    };
}

#endif //__CERTIFICATE_H__
