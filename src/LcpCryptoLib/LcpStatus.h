#ifndef __LCP_STATUS_H__
#define __LCP_STATUS_H__

namespace lcp
{
    struct StatusCover
    {
        enum StatusCode
        {
            // Error Common

            // No Error
            ErrorSuccess,
            // No NetProvider implementation has been given.
            ErrorNoNetProvider,
            // No StorageProvider implementation has been given.
            ErrorNoStorageProvider,

            // ErrorOpening

            // The given LCPL is not a valid License Document.
            ErrorLicenseNotValid,
            // The license hasn't begun yet (right 'start'),
            ErrorLicenseNotStarted,
            // The license is expired (right 'end'),
            ErrorLicenseExpired,
            // No Root Certificate provided by the Client.
            ErrorNoRootCertificate,
            // The Content Provider Certificate is not found in the root 
            // chain, or has been revoked.
            ErrorContentProviderCertificateNotValid,
            // The calculated License signature doesn't match the one 
            // provided by the License.
            ErrorLicenseSignatureNotValid,

            // ErrorAcquisition

            // No acquisition link found in the license.
            ErrorNoAcquisitionLink,
            // The downloaded publication doesn't match the license hash.
            ErrorPublicationCorrupted,

            // ErrorDecryption

            // The given User Pass phrase is not valid for this License
            ErrorUserPassphraseNotValid,
            // The License is still encrypted and can't be used to decrypt 
            // data.
            ErrorLicenseEncrypted,

            // ErrorNetworking

            // 404 Not found
            ErrorRequestNotFound,
            // Any other network error
            ErrorRequestFailed
        };
    };

    struct Status
    {
        StatusCover::StatusCode ResultCode;
        const char * Extension;
    };

    bool IsSuccess(Status status)
    {
        return status.ResultCode == StatusCover::ErrorSuccess;
    }
}

#endif //__LCP_STATUS_H__