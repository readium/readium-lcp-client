//
//  Created by Mickaël Menu on 02/11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#import <Foundation/Foundation.h>

extern NSString *const LCPErrorDomain;
extern NSString *const LCPErrorExtensionKey;

// Not an enum because these are actually aliases to the C++ enum values.
// We don't want to compile this header as an Objective-C++ file.
extern NSInteger const LCPErrorDecryptionLicenseEncrypted;
extern NSInteger const LCPErrorDecryptionUserPassphraseNotValid;

#ifdef __cplusplus
namespace lcp {
    class Status;
}

extern NSError *LCPErrorFromStatus(lcp::Status status);

#endif
