//
//  Created by Mickaël Menu on 02/11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
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
    return [NSString stringWithUTF8String:_nativeLicense->Id().c_str()];
}

- (BOOL)isDecrypted
{
    return _nativeLicense->Decrypted();
}

- (NSString *)userHint
{
    return [NSString stringWithUTF8String:_nativeLicense->Crypto()->UserKeyHint().c_str()];
}

@end
