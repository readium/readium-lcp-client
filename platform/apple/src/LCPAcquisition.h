//
//  Created by Mickaël Menu on 02/11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//

#import <Foundation/Foundation.h>

@class LCPAcquisition;

@protocol LCPAcquisitionDelegate
@optional

- (void)lcpAcquisitionDidStart:(LCPAcquisition *)acquisition;
- (void)lcpAcquisition:(LCPAcquisition *)acquisition didProgress:(float)progress;
- (void)lcpAcquisitionDidCancel:(LCPAcquisition *)acquisition;
- (void)lcpAcquisition:(LCPAcquisition *)acquisition didEnd:(BOOL)success error:(NSError *)error;

@end


@interface LCPAcquisition : NSObject

@property (readonly, nonatomic) NSString *publicationPath;
@property (readonly, nonatomic) NSString *suggestedFilename;

- (void)cancel;

@end
