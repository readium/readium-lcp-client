#ifndef __LCP_STATUS_H__
#define __LCP_STATUS_H__

#include <string>

#ifdef _DEBUG
#pragma optimize( "", off )
#endif

namespace lcp
{
    struct StCodeCover
    {
        enum StatusCode
        {
            // Error Common

            // No Error
            ErrorCommonSuccess,
            // Common Error
            ErrorCommonFail,
            // No NetProvider implementation has been given.
            ErrorCommonNoNetProvider,
            // No StorageProvider implementation has been given.
            ErrorCommonNoStorageProvider,
            // Implementation of the Encryption Profile from the
            // license file was not found
            ErrorCommonEncryptionProfileNotFound,
            // Algorithm from encryption profile doesn't match algorithm
            // from license file or encryption.xml
            ErrorCommonAlgorithmMismatch,
            
            // ErrorOpening

            // The given LCPL is not a valid License Document.
            ErrorOpeningLicenseNotValid,
            // The license hasn't begun yet (right 'start'),
            ErrorOpeningLicenseNotStarted,
            // The license is expired (right 'end'),
            ErrorOpeningLicenseExpired,
            // The calculated License signature doesn't match the one 
            // provided by the License.
            ErrorOpeningLicenseSignatureNotValid,
            // No Root Certificate provided by the Client.
            ErrorOpeningNoRootCertificate,
            // Root Certificate provided by the Client is not valid
            ErrorOpeningRootCertificateNotValid,
            // Algorithm of the Root Certificate for signing child
            // certificate was not found
            ErrorOpeningRootCertificateSignatureAlgorithmNotFound,
            // The Content Provider Certificate is not valid
            ErrorOpeningContentProviderCertificateNotValid,
            // The Content Provider Certificate was not found in the root 
            // chain or not verified because of the crypto error
            ErrorOpeningContentProviderCertificateNotVerified,
            // The Content Provider Certificate has been revoked.
            ErrorOpeningContentProviderCertificateRevoked,
            // The Content Provider Certificate hasn't begun yet
            ErrorOpeningContentProviderCertificateNotStarted,
            // The Content Provider Certificate is expired
            ErrorOpeningContentProviderCertificateExpired,

            // ErrorAcquisition

            // No acquisition link found in the license.
            ErrorAcquisitionNoAcquisitionLink,
            // The downloaded publication doesn't match the license hash.
            ErrorAcquisitionPublicationCorrupted,
            // Acquisition link has wrong type
            ErrorAcquisitionPublicationWrongType,
            // Cannot open file to write
            ErrorAcquisitionInvalidFilePath,

            // ErrorDecryption

            // The given User Pass phrase is not valid for this License
            ErrorDecryptionUserPassphraseNotValid,
            // The License is still encrypted and can't be used to decrypt data.
            ErrorDecryptionLicenseEncrypted,
            // The Publication is still encrypted and can't be used to decrypt data.
            ErrorDecryptionPublicationEncrypted,
            //
            ErrorDecryptionCommonError,

            // ErrorNetworking

            // 404 Not found
            ErrorNetworkingRequestNotFound,
            // Any other network error
            ErrorNetworkingRequestFailed
        };
    };

    struct Status
    {
        Status(StCodeCover::StatusCode resultCode, const std::string & extension = "")
            : ResultCode(resultCode)
            , Extension(extension)
        {
        }

        StCodeCover::StatusCode ResultCode;
        std::string Extension;

        static bool IsSuccess(const Status & status)
        {
            return status.ResultCode == StCodeCover::ErrorCommonSuccess;
        }
    };
}

#endif //__LCP_STATUS_H__