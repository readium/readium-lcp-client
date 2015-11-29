//
//  Created by Mickaël Menu on 02/11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//

#import <Foundation/Foundation.h>

#ifdef __cplusplus
namespace lcp {
    class Status;
}
#endif

static NSString *const LCPErrorDomain = @"com.mantano.lcp.error";
static NSString *const LCPErrorExtensionKey = @"LCPErrorExtension";

extern NSError *LCPErrorFromStatus(lcp::Status status);
