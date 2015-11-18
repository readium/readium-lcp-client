#ifndef __I_CERTIFICATE_H__
#define __I_CERTIFICATE_H__

#include <string>
#include <vector>

namespace lcp
{
    class ICertificate
    {
    public:
        virtual std::string SerialNumber() const = 0;
        virtual std::string NotBeforeDate() const = 0;
        virtual std::string NotAfterDate() const = 0;
        virtual std::vector<unsigned char> PublicKey() const = 0;
        virtual bool VerifyCertificate(ICertificate * rootCertificate) = 0;
        virtual bool VerifyMessage(const std::string & message, const std::string & hashBase64) = 0;
        virtual ~ICertificate() {}
    };
}

#endif //__I_CERTIFICATE_H__
