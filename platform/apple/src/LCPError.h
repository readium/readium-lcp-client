//
//  Created by Mickaël Menu on 02/11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#import <Foundation/Foundation.h>

extern NSString *const LCPErrorDomain;
extern NSString *const LCPErrorExtensionKey;

// Not an enum because these are actually aliases to the C++ enum values.
// We don't want to compile this header as an Objective-C++ file.
// @see LcpStatus.h for documentation
extern NSInteger const LCPErrorCommonFail;
extern NSInteger const LCPErrorCommonNoNetProvider;
extern NSInteger const LCPErrorCommonNoStorageProvider;
extern NSInteger const LCPErrorCommonEncryptionProfileNotFound;
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
extern NSInteger const LCPErrorAcquisitionNoAcquisitionLink;
extern NSInteger const LCPErrorAcquisitionPublicationCorrupted;
extern NSInteger const LCPErrorAcquisitionPublicationWrongType;
extern NSInteger const LCPErrorAcquisitionInvalidFilePath;
extern NSInteger const LCPErrorDecryptionUserPassphraseNotValid;
extern NSInteger const LCPErrorDecryptionLicenseEncrypted;
extern NSInteger const LCPErrorDecryptionPublicationEncrypted;
extern NSInteger const LCPErrorDecryptionCommonError;
extern NSInteger const LCPErrorNetworkingRequestNotFound;
extern NSInteger const LCPErrorNetworkingRequestFailed;

#ifdef __cplusplus
namespace lcp {
    class Status;
}

extern NSError *LCPErrorFromStatus(lcp::Status status);

#endif
