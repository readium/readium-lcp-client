//
//  Created by Mickaël Menu on 02/11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//
//  Objective-C bridge to the LCP service C++ library.
//

#import <Foundation/Foundation.h>

#ifdef __cplusplus
namespace lcp {
    class ILcpService;
    class INetProvider;
    class IStorageProvider;
}
#endif

@class LCPLicense;

@interface LCPService : NSObject

+ (instancetype)serviceWithRootCertificate:(NSString *)rootCertificate netProvider:(lcp::INetProvider *)netProvider storageProvider:(lcp::IStorageProvider *)storageProvider error:(NSError **)error;

- (instancetype)initWithRootCertificate:(NSString *)rootCertificate netProvider:(lcp::INetProvider *)netProvider storageProvider:(lcp::IStorageProvider *)storageProvider error:(NSError **)error NS_DESIGNATED_INITIALIZER;

- (lcp::ILcpService *)nativeService;

- (LCPLicense *)openLicense:(NSString *)licenseJSON error:(NSError **)error;
- (BOOL)decryptLicense:(LCPLicense *)license passphrase:(NSString *)passphrase error:(NSError **)error;

@end
