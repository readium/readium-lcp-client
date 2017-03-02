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

#import <Foundation/Foundation.h>

extern NSString *const LCPErrorDomain;
extern NSString *const LCPErrorExtensionKey;

// Not an enum because these are actually aliases to the C++ enum values.
// We don't want to compile this header as an Objective-C++ file.
// @see LcpStatus.h for documentation
extern NSInteger const LCPErrorCommonNoNetProvider;
extern NSInteger const LCPErrorCommonNoStorageProvider;
#if ENABLE_PROFILE_NAMES
extern NSInteger const LCPErrorCommonEncryptionProfileNotFound;
#endif //ENABLE_PROFILE_NAMES
extern NSInteger const LCPErrorCommonAlgorithmMismatch;
extern NSInteger const LCPErrorOpeningLicenseNotValid;
extern NSInteger const LCPErrorOpeningLicenseNotStarted;
extern NSInteger const LCPErrorOpeningLicenseExpired;
extern NSInteger const LCPErrorOpeningLicenseSignatureNotValid;
extern NSInteger const LCPErrorOpeningNoRootCertificate;
extern NSInteger const LCPErrorOpeningRootCertificateNotValid;
extern NSInteger const LCPErrorOpeningRootCertificateSignatureAlgorithmNotFound;
extern NSInteger const LCPErrorOpeningContentProviderCertificateNotValid;
extern NSInteger const LCPErrorOpeningContentProviderCertificateNotVerified;
extern NSInteger const LCPErrorOpeningContentProviderCertificateRevoked;
extern NSInteger const LCPErrorOpeningContentProviderCertificateNotStarted;
extern NSInteger const LCPErrorOpeningContentProviderCertificateExpired;
extern NSInteger const LCPErrorOpeningDuplicateLicenseInstance;
extern NSInteger const LCPErrorDecryptionUserPassphraseNotValid;
extern NSInteger const LCPErrorDecryptionLicenseEncrypted;
extern NSInteger const LCPErrorDecryptionPublicationEncrypted;
extern NSInteger const LCPErrorDecryptionCommonError;
extern NSInteger const LCPErrorNetworkingRequestNotFound;
extern NSInteger const LCPErrorNetworkingRequestFailed;

#if ENABLE_NET_PROVIDER_ACQUISITION
extern NSInteger const LCPErrorAcquisitionNoAcquisitionLink;

extern NSInteger const LCPErrorAcquisitionPublicationCorrupted;

extern NSInteger const LCPErrorAcquisitionPublicationWrongType;

extern NSInteger const LCPErrorAcquisitionInvalidFilePath;
#endif //ENABLE_NET_PROVIDER_ACQUISITION

#ifdef __cplusplus
namespace lcp {
    class Status;
}

extern NSError *LCPErrorFromStatus(lcp::Status status);

#endif
