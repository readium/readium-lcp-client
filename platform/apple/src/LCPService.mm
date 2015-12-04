//
//  Created by Mickaël Menu on 02/11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//

#import "LCPService.h"

#import "DefaultFileSystemProvider.h"
#import "ILcpService.h"
#import "ILicense.h"
#import "LcpServiceCreator.h"
#import "LCPAcquisition.h"
#import "LCPError.h"
#import "LCPLicense.h"
#import "LCPiOSNetProvider.h"
#import "LCPiOSStorageProvider.h"
#import <iostream>

#if DEBUG
#define LOG(msg) std::cout << "LCP: " << msg << std::endl
#else
#define LOG(msg)
#endif

@interface LCPService () {
    std::shared_ptr<lcp::ILcpService> _nativeService;
}
@end

@implementation LCPService

+ (instancetype)serviceWithRootCertificate:(NSString *)rootCertificate error:(NSError **)error
{
    return [[self alloc] initWithRootCertificate:rootCertificate error:error];
}


- (instancetype)init
{
    return [self initWithRootCertificate:nil error:NULL];
}

- (instancetype)initWithRootCertificate:(NSString *)rootCertificate error:(NSError **)error
{
    lcp::ILcpService *service = nullptr;
    lcp::LcpServiceCreator factory;
    lcp::IStorageProvider *storageProvider;
    lcp::INetProvider *netProvider;
    lcp::IFileSystemProvider *fileSystemProvider;
    
#if TARGET_OS_IPHONE
    storageProvider = new lcp::iOSStorageProvider();
    netProvider = new lcp::iOSNetProvider();
    fileSystemProvider = new lcp::DefaultFileSystemProvider();
#endif
    
    lcp::Status status = factory.CreateLcpService([rootCertificate UTF8String], netProvider, storageProvider, fileSystemProvider, &service);
    
    if (![self checkStatus:status error:error]) {
        return nil;
    }
    
    self = [super init];
    if (self) {
        _nativeService = std::shared_ptr<lcp::ILcpService>(service);
    }
    
    return self;
}

- (lcp::ILcpService *)nativeService
{
    return _nativeService ? _nativeService.get() : nullptr;
}

- (LCPLicense *)openLicense:(NSString *)licenseJSON error:(NSError **)error
{
    if (!licenseJSON)
        return nil;
    
    LCPLicense *license;
    
    lcp::ILicense *nativeLicense;
    lcp::Status status = _nativeService->OpenLicense([licenseJSON UTF8String], &nativeLicense);
    
    if ([self checkStatus:status error:error]) {
        license = [[LCPLicense alloc] initWithLicense:nativeLicense];
        LOG("Open license " << (nativeLicense->Decrypted() ? "(decrypted)" : "(still encrypted)"));
    } else {
        LOG("Failed to open license <" << status.ResultCode << ": " << status.Extension << ">");
    }
    
    return license;
}

- (BOOL)decryptLicense:(LCPLicense *)license passphrase:(NSString *)passphrase error:(NSError **)error
{
    if (license.isDecrypted)
        return YES;
    
    lcp::Status status = lcp::StCodeCover::ErrorDecryptionLicenseEncrypted;
    if (passphrase) {
        status = _nativeService->DecryptLicense(license.nativeLicense, [passphrase UTF8String]);
    }
    return [self checkStatus:status error:error];
}

- (LCPAcquisition *)createAcquisition:(LCPLicense *)license publicationPath:(NSString *)publicationPath error:(NSError **)error
{
    LCPAcquisition *acquisition;
    lcp::IAcquisition *nativeAcquisition;
    
    lcp::Status status = _nativeService->CreatePublicationAcquisition([publicationPath UTF8String], license.nativeLicense, &nativeAcquisition);
    if ([self checkStatus:status error:error]) {
        acquisition = [[LCPAcquisition alloc] initWithAcquisition:nativeAcquisition];
    }
    
    return acquisition;
}

- (BOOL)checkStatus:(lcp::Status)status error:(NSError **)error
{
    if (!lcp::Status::IsSuccess(status)) {
        if (error != NULL) {
            *error = LCPErrorFromStatus(status);
        }
        return NO;
    }
    
    return YES;
}

@end
