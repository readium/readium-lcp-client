//
//  Created by Mickaël Menu on 02/11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//

#import "LCPService.h"

#import "ILcpService.h"
#import "ILicense.h"
#import "LcpServiceCreator.h"
#import "LCPError.h"
#import "LCPLicense.h"

#import <iostream>

@interface LCPService () {
    std::shared_ptr<lcp::ILcpService> _nativeService;
}
@end

@implementation LCPService

+ (instancetype)serviceWithRootCertificate:(NSString *)rootCertificate netProvider:(lcp::INetProvider *)netProvider storageProvider:(lcp::IStorageProvider *)storageProvider error:(NSError **)error
{
    return [[self alloc] initWithRootCertificate:rootCertificate netProvider:netProvider storageProvider:storageProvider error:error];
}


- (instancetype)init
{
    return [self initWithRootCertificate:nil netProvider:nullptr storageProvider:nullptr error:NULL];
}

- (instancetype)initWithRootCertificate:(NSString *)rootCertificate netProvider:(lcp::INetProvider *)netProvider storageProvider:(lcp::IStorageProvider *)storageProvider error:(NSError **)error
{
    lcp::ILcpService *service = nullptr;
    lcp::LcpServiceCreator factory;
    lcp::Status status = factory.CreateLcpService([rootCertificate UTF8String], netProvider, storageProvider, nullptr, &service);
    
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
    }
    
    return license;
}

- (BOOL)decryptLicense:(LCPLicense *)license passphrase:(NSString *)passphrase error:(NSError **)error
{
    lcp::Status status = _nativeService->DecryptLicense(license.nativeLicense, [passphrase UTF8String]);
    return [self checkStatus:status error:error];
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
