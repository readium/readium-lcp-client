//
//  Created by Mickaël Menu on 02/11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#import <Foundation/Foundation.h>

#ifdef __cplusplus
namespace lcp {
    class ILicense;
}
#endif


@interface LCPLicense : NSObject

@property (readonly, nonatomic) NSString *identifier;
@property (readonly, nonatomic) BOOL isDecrypted;

@property (readonly, nonatomic) NSString *userHint;

#ifdef __cplusplus
@property (readonly, nonatomic) lcp::ILicense *nativeLicense;
- (instancetype)initWithLicense:(lcp::ILicense *)nativeLicense NS_DESIGNATED_INITIALIZER;
#endif

@end
