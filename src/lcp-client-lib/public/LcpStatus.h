//
//  Created by Mickaël Menu, Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#ifndef __LCP_STATUS_H__
#define __LCP_STATUS_H__

#include <string>

#ifdef _DEBUG
#pragma optimize( "", off )
#endif

namespace lcp
{
    struct StatusCode
    {
        enum StatusCodeEnum
        {
            //
            // Common errors
            //
            // No Error
            ErrorCommonSuccess,
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
            
            //
            // Errors when opening a License Document.
            //
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
            // Trying to save duplicate license instance while opening
            ErrorOpeningDuplicateLicenseInstance,

            //
            // Errors when acquiring a protected publication from a License.
            //
            // No acquisition link found in the license.
            ErrorAcquisitionNoAcquisitionLink,
            // The downloaded publication doesn't match the license hash.
            ErrorAcquisitionPublicationCorrupted,
            // Acquisition link has wrong type
            ErrorAcquisitionPublicationWrongType,
            // Cannot open file to write
            ErrorAcquisitionInvalidFilePath,

            //
            // Errors when decrypting a License or data.
            //
            // The given User Pass phrase is not valid for this License
            ErrorDecryptionUserPassphraseNotValid,
            // The License is still encrypted and can't be used to decrypt data.
            ErrorDecryptionLicenseEncrypted,
            // The Publication is still encrypted and can't be used to decrypt data.
            ErrorDecryptionPublicationEncrypted,
            // Error of crypto library
            ErrorDecryptionCommonError,

            //
            // Errors when doing HTTP network calls.
            //
            // 404 Not found
            ErrorNetworkingRequestNotFound,
            // Any other network error
            ErrorNetworkingRequestFailed

#if !DISABLE_LSD
            ,

            // License Status Document: invalid content-type declaration,
            // incorrect string value for 'type' field of LCP license 'links' > 'status' JSON property,
            // should be "application/vnd.readium.license.status.v1.0+json".
            ErrorStatusDocumentWrongType,

            // License Status Document: retrieval URI is not correct.
            ErrorStatusDocumentInvalidUri,

            // License Status Document: the downloaded file doesn't match the hash.
            ErrorStatusDocumentCorrupted,

            // License Status Document: incorrect syntax / format
            // for file retrived from the 'href' URI of 'type' "application/vnd.readium.license.status.v1.0+json".
            ErrorStatusDocumentNotValid,

            //ErrorStatusDocumentInvalidFilePath,

            ErrorStatusDocumentNewLicense
#endif //!DISABLE_LSD
        };
    };

    struct Status
    {
        Status(StatusCode::StatusCodeEnum code, const std::string & extension = "")
            : Code(code)
            , Extension(extension)
        {
        }

        StatusCode::StatusCodeEnum Code;
        std::string Extension;

        static bool IsSuccess(const Status & status)
        {
            return status.Code == StatusCode::ErrorCommonSuccess;
        }
    };
}

#endif //__LCP_STATUS_H__