#ifndef __I_CRYPTO_H__
#define __I_CRYPTO_H__

#include <string>

namespace lcp
{
    class ICrypto
    {
    public:
        virtual std::string EncryptionProfile() const = 0;
        virtual std::string ContentKey() const = 0;
        virtual std::string ContentKeyAlgorithm() const = 0;
        virtual std::string UserKeyHint() const = 0;
        virtual std::string UserKeyAlgorithm() const = 0;
        virtual std::string UserKeyCheck() const = 0;
        virtual std::string Signature() const = 0;
        virtual std::string SignatureAlgorithm() const = 0;
        virtual std::string SignatureCertificate() const = 0;
        virtual ~ICrypto() {}
    };
}

#endif //__I_CRYPTO_H__
