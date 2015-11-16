#ifndef __I_CRYPTO_PROVIDER_H__
#define __I_CRYPTO_PROVIDER_H__

#include "Public/LcpStatus.h"

namespace lcp
{
    class ICrypto;
    class IRights;

    class ICryptoProvider
    {
    public:
        virtual Status Validate(
            const std::string & rootCertificate,
            const std::string & canonicalLicense,
            ICrypto * crypto,
            IRights * rights
            ) = 0;

        virtual ~ICryptoProvider() {}
    };
}

#endif //__I_CRYPTO_PROVIDER_H__