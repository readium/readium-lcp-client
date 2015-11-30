//
//  Created by Mickaël Menu on 02/11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//

#import <Foundation/Foundation.h>

#ifdef __cplusplus
namespace lcp {
    class IAcquisition;
}
#endif

@class LCPAcquisition;

@protocol LCPAcquisitionDelegate <NSObject>
@optional

- (void)lcpAcquisitionDidStart:(LCPAcquisition *)acquisition;
- (void)lcpAcquisition:(LCPAcquisition *)acquisition didProgress:(float)progress;
- (void)lcpAcquisitionDidCancel:(LCPAcquisition *)acquisition;
- (void)lcpAcquisition:(LCPAcquisition *)acquisition didEnd:(BOOL)success error:(NSError *)error;

@end


@interface LCPAcquisition : NSObject

@property (weak, nonatomic) id<LCPAcquisitionDelegate> delegate;

@property (readonly, nonatomic) NSString *publicationPath;
@property (readonly, nonatomic) NSString *suggestedFilename;

#ifdef __cplusplus
@property (readonly, nonatomic) lcp::IAcquisition *nativeAcquisition;
- (instancetype)initWithAcquisition:(lcp::IAcquisition *)nativeAcquisition NS_DESIGNATED_INITIALIZER;
#endif

- (void)startWithDelegate:(id<LCPAcquisitionDelegate>)delegate;
- (void)cancel;

@end
