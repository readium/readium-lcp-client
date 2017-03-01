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

#import "LCPService.h"

#import "DefaultFileSystemProvider.h"
#import "ILcpService.h"
#import "ILicense.h"
#import "IRights.h"
#import "IRightsService.h"
#import "LcpServiceCreator.h"
#if ENABLE_NET_PROVIDER_ACQUISITION
#import "LCPAcquisition.h"
#endif //ENABLE_NET_PROVIDER_ACQUISITION
#import "LCPError.h"
#import "LCPLicense.h"
#if !DISABLE_NET_PROVIDER
#import "LCPiOSNetProvider.h"
#endif //!DISABLE_NET_PROVIDER
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
#if !DISABLE_NET_PROVIDER
    INetProvider *netProvider;
#endif //!DISABLE_NET_PROVIDER
    IFileSystemProvider *fileSystemProvider;
    
#if true //TARGET_OS_IPHONE (WORKS FINE FOR BOTH iOS and OSX)
    storageProvider = new iOSStorageProvider();
#if !DISABLE_NET_PROVIDER
    netProvider = new iOSNetProvider();
#endif //!DISABLE_NET_PROVIDER
    fileSystemProvider = new DefaultFileSystemProvider();
#endif
    
    Status status = factory.CreateLcpService([rootCertificate UTF8String],
#if !DISABLE_NET_PROVIDER
                                             netProvider,
#endif //!DISABLE_NET_PROVIDER
                                             storageProvider,
                                             fileSystemProvider,
                                             &service
#if !DISABLE_CRL
                                             , std::string()
#endif //!DISABLE_CRL
                                             );
    
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

- (void)injectLicense:(NSString *)epubPath licenseJSON:(NSString *)licenseJSON
{
    _nativeService->InjectLicense(std::string([epubPath UTF8String]), std::string([licenseJSON UTF8String]));
}

- (LCPLicense *)openLicense:(NSString *)path licenseJSON:(NSString *)licenseJSON error:(NSError **)error
 {
     if (!licenseJSON)
         return nil;
    
     LCPLicense *license;
    
     ILicense *nativeLicense;
     Status status = _nativeService->OpenLicense([path UTF8String], [licenseJSON UTF8String], &nativeLicense);
    
     if ([self checkStatus:status error:error]) {
         license = [[LCPLicense alloc] initWithLicense:nativeLicense];
         LOG("Open license " << (nativeLicense->Decrypted() ? "(decrypted)" : "(still encrypted)"));
     } else {
         LOG("Failed to open license <" << status.Code << ": " << status.Extension << ">");
     }
    
     return license;
 }

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

#if ENABLE_NET_PROVIDER_ACQUISITION
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
#endif //ENABLE_NET_PROVIDER_ACQUISITION

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
