//
//  Created by Mickaël Menu on 02/11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//

#import "LCPLicense.h"

#import "ICrypto.h"
#import "ILicense.h"

@interface LCPLicense ()
@property (nonatomic) lcp::ILicense *nativeLicense;

@end


@implementation LCPLicense

- (instancetype)init
{
    return [self initWithLicense:nullptr];
}

- (instancetype)initWithLicense:(lcp::ILicense *)nativeLicense
{
    self = [super init];
    if (self) {
        _nativeLicense = nativeLicense;
    }
    
    return self;
}

- (NSString *)identifier
{
    return [self stringFromNativeString:_nativeLicense->Id()];
}

- (BOOL)isDecrypted
{
    return _nativeLicense->Decrypted();
}

- (NSString *)userHint
{
    return [self stringFromNativeString:_nativeLicense->Crypto()->UserKeyHint()];
}

- (NSString *)stringFromNativeString:(std::string)nativeString
{
    return [NSString stringWithCString:nativeString.c_str() encoding:NSUTF8StringEncoding];
}

@end
