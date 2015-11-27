//
//  Created by Mickaël Menu on 27/11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//

#import "LcpStatus.h"
#import "LCPError.h"

NSError *LCPErrorFromStatus(lcp::Status status)
{
    if (lcp::Status::IsSuccess(status)) {
        return nil;
    }
    
    NSDictionary *userInfo;
    if (!status.Extension.empty()) {
        userInfo = @{
            LCPErrorExtensionKey: [NSString stringWithUTF8String:status.Extension.c_str()]
        };
    }
    
    return [[NSError alloc] initWithDomain:LCPErrorDomain code:status.ResultCode userInfo:userInfo];
}
