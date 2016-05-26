// Copyright (c) 2016 Mantano
// Licensed to the Readium Foundation under one or more contributor license agreements.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation and/or
//    other materials provided with the distribution.
// 3. Neither the name of the organization nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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