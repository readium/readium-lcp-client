#include "CryptoppCryptoProvider.h"
#include "Public/ILicense.h"
#include "Public/ICrypto.h"
#include "Public/IRights.h"
#include "Certificate.h"
#include "DateTime.h"

namespace lcp
{
    Status CryptoppCryptoProvider::VerifyLicense(
        const std::string & rootCertificateBase64,
        const std::string & canonicalLicense,
        ILicense * license
        )
    {
        try
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

            if (!providerCertificate->VerifyCertificate(rootCertificate.get()))
            {
                return Status(StCodeCover::ErrorOpeningContentProviderCertificateNotVerified);
            }
            if (!providerCertificate->VerifyMessage(canonicalLicense, license->Crypto()->Signature()))
            {
                return Status(StCodeCover::ErrorOpeningLicenseSignatureNotValid);
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
                return Status(StCodeCover::ErrorOpeningContentProviderCertificateNotStarted);
            }
            else if (lastUpdated > notAfter)
            {
                return Status(StCodeCover::ErrorOpeningContentProviderCertificateExpired);
            }
            return Status(StCodeCover::ErrorCommonSuccess);
        }
        catch (const CryptoPP::Exception & ex)
        {
            return Status(StCodeCover::ErrorOpeningCryptoError);
        }
    }
}