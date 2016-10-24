//
//  Created by Mickaël Menu on 02/11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#import "LCPService.h"

#import "DefaultFileSystemProvider.h"
#import "ILcpService.h"
#import "ILicense.h"
#import "IRights.h"
#import "IRightsService.h"
#import "LcpServiceCreator.h"
#if ENABLE_NET_PROVIDER
#import "LCPAcquisition.h"
#endif //ENABLE_NET_PROVIDER
#import "LCPError.h"
#import "LCPLicense.h"
#if ENABLE_NET_PROVIDER
#import "LCPiOSNetProvider.h"
#endif //ENABLE_NET_PROVIDER
#import "LCPiOSStorageProvider.h"
#import <iostream>

#if DEBUG
#define LOG(msg) std::cout << "LCP: " << msg << std::endl
#else
#define LOG(msg)
#endif

using namespace lcp;

NSString *const LCPRightPrint = @(PrintRight);
NSString *const LCPRightCopy = @(CopyRight);
NSString *const LCPRightTTS = @(TtsRight);
NSString *const LCPRightStart = @(StartRight);
NSString *const LCPRightEnd = @(EndRight);


@interface LCPService () {
    std::shared_ptr<ILcpService> _nativeService;
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
    ILcpService *service = nullptr;
    LcpServiceCreator factory;
    IStorageProvider *storageProvider;
    INetProvider *netProvider;
    IFileSystemProvider *fileSystemProvider;
    
#if true //TARGET_OS_IPHONE (WORKS FINE FOR BOTH iOS and OSX)
    storageProvider = new iOSStorageProvider();
#if ENABLE_NET_PROVIDER
    netProvider = new iOSNetProvider();
#endif //ENABLE_NET_PROVIDER
    fileSystemProvider = new DefaultFileSystemProvider();
#endif
    
    Status status = factory.CreateLcpService([rootCertificate UTF8String],
#if ENABLE_NET_PROVIDER
                                             netProvider,
#endif //ENABLE_NET_PROVIDER
                                             storageProvider, fileSystemProvider, &service);
    
    if (![self checkStatus:status error:error]) {
        return nil;
    }
    
    self = [super init];
    if (self) {
        _nativeService = std::shared_ptr<ILcpService>(service);
    }
    
    return self;
}

- (ILcpService *)nativeService
{
    return _nativeService ? _nativeService.get() : nullptr;
}

// - (LCPLicense *)openLicense:(NSString *)licenseJSON error:(NSError **)error
// {
//     if (!licenseJSON)
//         return nil;
    
//     LCPLicense *license;
    
//     ILicense *nativeLicense;
//     Status status = _nativeService->OpenLicense([licenseJSON UTF8String], &nativeLicense);
    
//     if ([self checkStatus:status error:error]) {
//         license = [[LCPLicense alloc] initWithLicense:nativeLicense];
//         LOG("Open license " << (nativeLicense->Decrypted() ? "(decrypted)" : "(still encrypted)"));
//     } else {
//         LOG("Failed to open license <" << status.Code << ": " << status.Extension << ">");
//     }
    
//     return license;
// }

- (BOOL)decryptLicense:(LCPLicense *)license passphrase:(NSString *)passphrase error:(NSError **)error
{
    if (license.isDecrypted)
        return YES;
    
    Status status = StatusCode::ErrorDecryptionLicenseEncrypted;
    if (passphrase) {
        status = _nativeService->DecryptLicense(license.nativeLicense, [passphrase UTF8String]);
    }
    return [self checkStatus:status error:error];
}

#if ENABLE_NET_PROVIDER
- (LCPAcquisition *)createAcquisition:(LCPLicense *)license publicationPath:(NSString *)publicationPath error:(NSError **)error
{
    LCPAcquisition *acquisition;
    IAcquisition *nativeAcquisition;
    
    Status status = _nativeService->CreatePublicationAcquisition([publicationPath UTF8String], license.nativeLicense, &nativeAcquisition);
    if ([self checkStatus:status error:error]) {
        acquisition = [[LCPAcquisition alloc] initWithAcquisition:nativeAcquisition];
    }
    
    return acquisition;
}
#endif //ENABLE_NET_PROVIDER

- (BOOL)checkStatus:(Status)status error:(NSError **)error
{
    if (!Status::IsSuccess(status)) {
        if (error != NULL) {
            *error = LCPErrorFromStatus(status);
        }
        return NO;
    }
    
    return YES;
}

- (BOOL)canUseRight:(NSString *)rightIdentifier license:(LCPLicense *)license
{
    return self.nativeService->GetRightsService()->CanUseRight(license.nativeLicense, [rightIdentifier UTF8String]);
}

- (BOOL)useRight:(NSString *)rightIdentifier license:(LCPLicense *)license
{
    return self.nativeService->GetRightsService()->UseRight(license.nativeLicense, [rightIdentifier UTF8String]);
}

- (BOOL)useRight:(NSString *)rightIdentifier amount:(NSInteger)amount license:(LCPLicense *)license
{
    return self.nativeService->GetRightsService()->UseRight(license.nativeLicense, [rightIdentifier UTF8String], (int)amount);
}

- (NSString *)valueForRight:(NSString *)rightIdentifier license:(LCPLicense *)license
{
    std::string value = self.nativeService->GetRightsService()->GetValue(license.nativeLicense, [rightIdentifier UTF8String]);
    return [NSString stringWithUTF8String:value.c_str()];
}

- (void)setValue:(NSString *)value forRight:(NSString *)rightIdentifier license:(LCPLicense *)license
{
    self.nativeService->GetRightsService()->SetValue(license.nativeLicense, [rightIdentifier UTF8String], [value UTF8String]);
}

@end
