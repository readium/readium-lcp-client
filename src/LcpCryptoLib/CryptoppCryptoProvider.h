#ifndef __CRYPTOPP_CRYPTO_PROVIDER_H__
#define __CRYPTOPP_CRYPTO_PROVIDER_H__

#include "ICryptoProvider.h"

namespace lcp
{
    class CryptoppCryptoProvider : public ICryptoProvider
    {
    public:
        virtual Status Validate(
            const std::string & rootCertificate,
            const std::string & canonicalLicense,
            ICrypto * crypto,
            IRights * rights
            );
    };
}

#endif //__CRYPTOPP_CRYPTO_PROVIDER_H__
