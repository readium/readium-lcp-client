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
            // No NetProvider implementation has been given.
            ErrorCommonNoNetProvider,
            // No StorageProvider implementation has been given.
            ErrorCommonNoStorageProvider,

            // ErrorOpening

            // The given LCPL is not a valid License Document.
            ErrorOpeningLicenseNotValid,
            // The license hasn't begun yet (right 'start'),
            ErrorOpeningLicenseNotStarted,
            // The license is expired (right 'end'),
            ErrorOpeningLicenseExpired,
            // No Root Certificate provided by the Client.
            ErrorOpeningNoRootCertificate,
            // The Content Provider Certificate is not found in the root 
            // chain, or has been revoked.
            ErrorOpeningContentProviderCertificateNotValid,
            // The calculated License signature doesn't match the one 
            // provided by the License.
            ErrorOpeningLicenseSignatureNotValid,

            // ErrorAcquisition

            // No acquisition link found in the license.
            ErrorAcquisitionNoAcquisitionLink,
            // The downloaded publication doesn't match the license hash.
            ErrorAcquisitionPublicationCorrupted,

            // ErrorDecryption

            // The given User Pass phrase is not valid for this License
            ErrorDecryptionUserPassphraseNotValid,
            // The License is still encrypted and can't be used to decrypt 
            // data.
            ErrorDecryptionLicenseEncrypted,

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