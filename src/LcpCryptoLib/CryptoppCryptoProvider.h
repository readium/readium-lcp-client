#ifndef __CRYPTOPP_CRYPTO_PROVIDER_H__
#define __CRYPTOPP_CRYPTO_PROVIDER_H__

#include "ICryptoProvider.h"

namespace lcp
{
    class CryptoppCryptoProvider : public ICryptoProvider
    {
    public:
        virtual Status VerifyLicense(
            const std::string & rootCertificateBase64,
            const std::string & canonicalLicense,
            ILicense * license
            );
    };
}

#endif //__CRYPTOPP_CRYPTO_PROVIDER_H__
