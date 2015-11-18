#ifndef __I_CRYPTO_PROVIDER_H__
#define __I_CRYPTO_PROVIDER_H__

#include "Public/LcpStatus.h"

namespace lcp
{
    class ILicense;

    class ICryptoProvider
    {
    public:
        virtual Status Validate(
            const std::string & rootCertificateBase64,
            const std::string & canonicalLicense,
            ILicense * licenses
            ) = 0;

        virtual ~ICryptoProvider() {}
    };
}

#endif //__I_CRYPTO_PROVIDER_H__