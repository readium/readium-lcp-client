//
//  Created by Mickaël Menu on 02/11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#import <Foundation/Foundation.h>

#ifdef __cplusplus
namespace lcp {
    class ILcpService;
#if ENABLE_NET_PROVIDER
    class INetProvider;
#endif //ENABLE_NET_PROVIDER
    class IStorageProvider;
}
#endif

#if ENABLE_NET_PROVIDER
@class LCPAcquisition;
#endif //ENABLE_NET_PROVIDER

@class LCPLicense;

extern NSString *const LCPRightPrint;
extern NSString *const LCPRightCopy;
extern NSString *const LCPRightTTS;
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

#if ENABLE_NET_PROVIDER
- (LCPAcquisition *)createAcquisition:(LCPLicense *)license publicationPath:(NSString *)publicationPath error:(NSError **)error;
#endif //ENABLE_NET_PROVIDER


- (BOOL)canUseRight:(NSString *)rightIdentifier license:(LCPLicense *)license;
- (BOOL)useRight:(NSString *)rightIdentifier license:(LCPLicense *)license;
- (BOOL)useRight:(NSString *)rightIdentifier amount:(NSInteger)amount license:(LCPLicense *)license;
- (NSString *)valueForRight:(NSString *)rightIdentifier license:(LCPLicense *)license;
- (void)setValue:(NSString *)value forRight:(NSString *)rightIdentifier license:(LCPLicense *)license;

@end
