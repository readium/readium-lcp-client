//
//  Created by Mickaël Menu on 27/11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#import "LcpStatus.h"
#import "LCPError.h"

NSString *const LCPErrorDomain = @"com.mantano.lcp.error";
NSString *const LCPErrorExtensionKey = @"LCPErrorExtension";

using namespace lcp;

NSInteger const LCPErrorCommonNoNetProvider = StatusCode::ErrorCommonNoNetProvider;
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
NSInteger const LCPErrorAcquisitionNoAcquisitionLink = StatusCode::ErrorAcquisitionNoAcquisitionLink;
NSInteger const LCPErrorAcquisitionPublicationCorrupted = StatusCode::ErrorAcquisitionPublicationCorrupted;
NSInteger const LCPErrorAcquisitionPublicationWrongType = StatusCode::ErrorAcquisitionPublicationWrongType;
NSInteger const LCPErrorAcquisitionInvalidFilePath = StatusCode::ErrorAcquisitionInvalidFilePath;
NSInteger const LCPErrorDecryptionUserPassphraseNotValid = StatusCode::ErrorDecryptionUserPassphraseNotValid;
NSInteger const LCPErrorDecryptionLicenseEncrypted = StatusCode::ErrorDecryptionLicenseEncrypted;
NSInteger const LCPErrorDecryptionPublicationEncrypted = StatusCode::ErrorDecryptionPublicationEncrypted;
NSInteger const LCPErrorDecryptionCommonError = StatusCode::ErrorDecryptionCommonError;
NSInteger const LCPErrorNetworkingRequestNotFound = StatusCode::ErrorNetworkingRequestNotFound;
NSInteger const LCPErrorNetworkingRequestFailed = StatusCode::ErrorNetworkingRequestFailed;

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
