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

#import "LcpStatus.h"
#import "LCPError.h"

NSString *const LCPErrorDomain = @"org.readium.lcp.error";
NSString *const LCPErrorExtensionKey = @"LCPErrorExtension";

using namespace lcp;

#if ENABLE_NET_PROVIDER
NSInteger const LCPErrorCommonNoNetProvider = StatusCode::ErrorCommonNoNetProvider;
#endif //ENABLE_NET_PROVIDER
NSInteger const LCPErrorCommonNoStorageProvider = StatusCode::ErrorCommonNoStorageProvider;
NSInteger const LCPErrorCommonEncryptionProfileNotFound = StatusCode::ErrorCommonEncryptionProfileNotFound;
NSInteger const LCPErrorCommonAlgorithmMismatch = StatusCode::ErrorCommonAlgorithmMismatch;
NSInteger const LCPErrorOpeningLicenseNotValid = StatusCode::ErrorOpeningLicenseNotValid;
NSInteger const LCPErrorOpeningLicenseNotStarted = StatusCode::ErrorOpeningLicenseNotStarted;
NSInteger const LCPErrorOpeningLicenseExpired = StatusCode::ErrorOpeningLicenseExpired;
NSInteger const LCPErrorOpeningLicenseSignatureNotValid = StatusCode::ErrorOpeningLicenseSignatureNotValid;
NSInteger const LCPErrorOpeningNoRootCertificate = StatusCode::ErrorOpeningNoRootCertificate;
NSInteger const LCPErrorOpeningRootCertificateNotValid = StatusCode::ErrorOpeningRootCertificateNotValid;
NSInteger const LCPErrorOpeningRootCertificateSignatureAlgorithmNotFound = StatusCode::ErrorOpeningRootCertificateSignatureAlgorithmNotFound;
NSInteger const LCPErrorOpeningContentProviderCertificateNotValid = StatusCode::ErrorOpeningContentProviderCertificateNotValid;
NSInteger const LCPErrorOpeningContentProviderCertificateNotVerified = StatusCode::ErrorOpeningContentProviderCertificateNotVerified;
NSInteger const LCPErrorOpeningContentProviderCertificateRevoked = StatusCode::ErrorOpeningContentProviderCertificateRevoked;
NSInteger const LCPErrorOpeningContentProviderCertificateNotStarted = StatusCode::ErrorOpeningContentProviderCertificateNotStarted;
NSInteger const LCPErrorOpeningContentProviderCertificateExpired = StatusCode::ErrorOpeningContentProviderCertificateExpired;
NSInteger const LCPErrorOpeningDuplicateLicenseInstance = StatusCode::ErrorOpeningDuplicateLicenseInstance;
#if ENABLE_NET_PROVIDER
NSInteger const LCPErrorAcquisitionNoAcquisitionLink = StatusCode::ErrorAcquisitionNoAcquisitionLink;
NSInteger const LCPErrorAcquisitionPublicationCorrupted = StatusCode::ErrorAcquisitionPublicationCorrupted;
NSInteger const LCPErrorAcquisitionPublicationWrongType = StatusCode::ErrorAcquisitionPublicationWrongType;
NSInteger const LCPErrorAcquisitionInvalidFilePath = StatusCode::ErrorAcquisitionInvalidFilePath;
#endif //ENABLE_NET_PROVIDER
NSInteger const LCPErrorDecryptionUserPassphraseNotValid = StatusCode::ErrorDecryptionUserPassphraseNotValid;
NSInteger const LCPErrorDecryptionLicenseEncrypted = StatusCode::ErrorDecryptionLicenseEncrypted;
NSInteger const LCPErrorDecryptionPublicationEncrypted = StatusCode::ErrorDecryptionPublicationEncrypted;
NSInteger const LCPErrorDecryptionCommonError = StatusCode::ErrorDecryptionCommonError;
#if ENABLE_NET_PROVIDER
NSInteger const LCPErrorNetworkingRequestNotFound = StatusCode::ErrorNetworkingRequestNotFound;
NSInteger const LCPErrorNetworkingRequestFailed = StatusCode::ErrorNetworkingRequestFailed;

#endif //ENABLE_NET_PROVIDER

NSError *LCPErrorFromStatus(Status status)
{
    if (Status::IsSuccess(status)) {
        return nil;
    }
    
    NSDictionary *userInfo;
    if (!status.Extension.empty()) {
        userInfo = @{
            LCPErrorExtensionKey: [NSString stringWithUTF8String:status.Extension.c_str()]
        };
    }
    
    return [[NSError alloc] initWithDomain:LCPErrorDomain code:status.Code userInfo:userInfo];
}
