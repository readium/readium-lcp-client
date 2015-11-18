#include "CryptoppCryptoProvider.h"
#include "Public/ILicense.h"
#include "Public/ICrypto.h"
#include "Public/IRights.h"
#include "Certificate.h"
#include "DateTime.h"

namespace lcp
{
    Status CryptoppCryptoProvider::Validate(
        const std::string & rootCertificateBase64,
        const std::string & canonicalLicense,
        ILicense * license
        )
    {
        if (rootCertificateBase64.empty())
        {
            return Status(StCodeCover::ErrorOpeningNoRootCertificate);
        }

        std::unique_ptr<Certificate> rootCertificate;
        try
        {
            rootCertificate.reset(new Certificate(rootCertificateBase64));
        }
        catch (CryptoPP::BERDecodeErr & ex)
        {
            return Status(StCodeCover::ErrorOpeningRootCertificateNotValid, ex.GetWhat());
        }

        std::unique_ptr<Certificate> providerCertificate;
        try
        {
            providerCertificate.reset(new Certificate(license->Crypto()->SignatureCertificate()));
        }
        catch (CryptoPP::BERDecodeErr & ex)
        {
            return Status(StCodeCover::ErrorOpeningContentProviderCertificateNotValid, ex.GetWhat());
        }

        DateTime notBefore(providerCertificate->NotBeforeDate());
        DateTime notAfter(providerCertificate->NotAfterDate());

        DateTime lastUpdated;
        if (!license->Updated().empty())
        {
            lastUpdated = DateTime(license->Updated());
        }
        else
        {
            lastUpdated = DateTime(license->Issued());
        }

        if (lastUpdated < notBefore)
        {
            return Status(StCodeCover::ErrorOpeningLicenseNotStarted);
        }
        else if (lastUpdated > notAfter)
        {
            return Status(StCodeCover::ErrorOpeningLicenseExpired);
        }

        if (!providerCertificate->VerifyCertificate(rootCertificate.get()))
        {
            return Status(StCodeCover::ErrorOpeningContentProviderCertificateNotVerified);
        }
        if (!providerCertificate->ValidateMessage(canonicalLicense, license->Crypto()->Signature()))
        {
            return Status(StCodeCover::ErrorOpeningLicenseSignatureNotValid);
        }

        return Status(StCodeCover::ErrorCommonSuccess);
    }
}