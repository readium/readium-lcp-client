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

@class LCPAcquisition;
@class LCPLicense;

@interface LCPService : NSObject

+ (instancetype)serviceWithRootCertificate:(NSString *)rootCertificate error:(NSError **)error;

- (instancetype)initWithRootCertificate:(NSString *)rootCertificate error:(NSError **)error NS_DESIGNATED_INITIALIZER;

#ifdef __cplusplus
- (lcp::ILcpService *)nativeService;
#endif

- (LCPLicense *)openLicense:(NSString *)licenseJSON error:(NSError **)error;
- (BOOL)decryptLicense:(LCPLicense *)license passphrase:(NSString *)passphrase error:(NSError **)error;

- (LCPAcquisition *)acquirePublication:(LCPLicense *)license toPath:(NSString *)publicationPath error:(NSError **)error;

@end
