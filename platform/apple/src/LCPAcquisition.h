//
//  Created by Mickaël Menu on 02/11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//  Licensed under Gnu Affero General Public License Version 3 (provided, notwithstanding this notice,
//  Readium Foundation reserves the right to license this material under a different separate license,
//  and if you have done so, the terms of that separate license control and the following references
//  to GPL do not apply).
//
//  This program is free software: you can redistribute it and/or modify it under the terms of the GNU
//  Affero General Public License as published by the Free Software Foundation, either version 3 of
//  the License, or (at your option) any later version. You should have received a copy of the GNU
//  Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
