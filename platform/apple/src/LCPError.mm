//
//  Created by Mickaël Menu on 27/11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#import "LcpStatus.h"
#import "LCPError.h"

NSString *const LCPErrorDomain = @"com.mantano.lcp.error";
NSString *const LCPErrorExtensionKey = @"LCPErrorExtension";

using namespace lcp;

NSInteger const LCPErrorDecryptionLicenseEncrypted = StatusCode::ErrorDecryptionLicenseEncrypted;
NSInteger const LCPErrorDecryptionUserPassphraseNotValid = StatusCode::ErrorDecryptionUserPassphraseNotValid;

NSError *LCPErrorFromStatus(Status status)
{
    if (Status::IsSuccess(status)) {
        return nil;
    }
    
    NSDictionary *userInfo;
    if (!status.Extension.empty()) {
        userInfo = @{
            LCPErrorExtensionKey: [NSString stringWithUTF8String:status.Extension.c_str()]
        };
    }
    
    return [[NSError alloc] initWithDomain:LCPErrorDomain code:status.Code userInfo:userInfo];
}
