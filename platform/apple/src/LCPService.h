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

#ifdef __cplusplus
namespace lcp {
    class ILcpService;
#if !DISABLE_NET_PROVIDER
    class INetProvider;
#endif //!DISABLE_NET_PROVIDER
    class IStorageProvider;
}
#endif

#if ENABLE_NET_PROVIDER_ACQUISITION
@class LCPAcquisition;
#endif //ENABLE_NET_PROVIDER_ACQUISITION

@class LCPLicense;

extern NSString *const LCPRightPrint;
extern NSString *const LCPRightCopy;
extern NSString *const LCPRightStart;
extern NSString *const LCPRightEnd;

@interface LCPService : NSObject

+ (instancetype)serviceWithRootCertificate:(NSString *)rootCertificate error:(NSError **)error;

- (instancetype)initWithRootCertificate:(NSString *)rootCertificate error:(NSError **)error NS_DESIGNATED_INITIALIZER;

#ifdef __cplusplus
- (lcp::ILcpService *)nativeService;
#endif

- (void)injectLicense:(NSString *)epubPath licenseJSON:(NSString *)licenseJSON;
- (LCPLicense *)openLicense:(NSString *)path licenseJSON:(NSString *)licenseJSON error:(NSError **)error;
- (BOOL)decryptLicense:(LCPLicense *)license passphrase:(NSString *)passphrase error:(NSError **)error;

#if ENABLE_NET_PROVIDER_ACQUISITION
- (LCPAcquisition *)createAcquisition:(LCPLicense *)license publicationPath:(NSString *)publicationPath error:(NSError **)error;
#endif //ENABLE_NET_PROVIDER_ACQUISITION


- (BOOL)canUseRight:(NSString *)rightIdentifier license:(LCPLicense *)license;
- (BOOL)useRight:(NSString *)rightIdentifier license:(LCPLicense *)license;
- (BOOL)useRight:(NSString *)rightIdentifier amount:(NSInteger)amount license:(LCPLicense *)license;
- (NSString *)valueForRight:(NSString *)rightIdentifier license:(LCPLicense *)license;
- (void)setValue:(NSString *)value forRight:(NSString *)rightIdentifier license:(LCPLicense *)license;

@end
