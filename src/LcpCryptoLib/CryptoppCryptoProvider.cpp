#include "CryptoppCryptoProvider.h"
#include "Public/ICrypto.h"
#include "Public/IRights.h"
#include "Certificate.h"

namespace lcp
{
    Status CryptoppCryptoProvider::Validate(
        const std::string & rootCertificate,
        const std::string & canonicalLicense,
        ICrypto * crypto,
        IRights * rights
        )
    {
        Certificate root(rootCertificate);
        Certificate providerCert(crypto->SignatureCertificate());

        if (providerCert.ValidateCertificate(&root))
        {
            if (providerCert.ValidateMessage(canonicalLicense, crypto->Signature()))
            {
                return Status(StCodeCover::ErrorCommonSuccess);
            }
        }
        return Status(StCodeCover::ErrorOpeningLicenseSignatureNotValid);
    }
}