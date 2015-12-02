//
//  Created by Mickaël Menu on 02/11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//

#import <Foundation/Foundation.h>

static NSString *const LCPErrorDomain = @"com.mantano.lcp.error";
static NSString *const LCPErrorExtensionKey = @"LCPErrorExtension";

#ifdef __cplusplus
namespace lcp {
    class Status;
}

extern NSError *LCPErrorFromStatus(lcp::Status status);

#endif
