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
    class ILcpService;
    class INetProvider;
    class IStorageProvider;
}
#endif

@class LCPAcquisition;
@class LCPLicense;

extern NSString *const LCPRightPrint;
extern NSString *const LCPRightCopy;
extern NSString *const LCPRightTTS;
extern NSString *const LCPRightStart;
extern NSString *const LCPRightEnd;

@interface LCPService : NSObject

+ (instancetype)serviceWithRootCertificate:(NSString *)rootCertificate error:(NSError **)error;

- (instancetype)initWithRootCertificate:(NSString *)rootCertificate error:(NSError **)error NS_DESIGNATED_INITIALIZER;

#ifdef __cplusplus
- (lcp::ILcpService *)nativeService;
#endif

- (LCPLicense *)openLicense:(NSString *)licenseJSON error:(NSError **)error;
- (BOOL)decryptLicense:(LCPLicense *)license passphrase:(NSString *)passphrase error:(NSError **)error;

- (LCPAcquisition *)createAcquisition:(LCPLicense *)license publicationPath:(NSString *)publicationPath error:(NSError **)error;


- (BOOL)canUseRight:(NSString *)rightIdentifier license:(LCPLicense *)license;
- (BOOL)useRight:(NSString *)rightIdentifier license:(LCPLicense *)license;
- (BOOL)useRight:(NSString *)rightIdentifier amount:(NSInteger)amount license:(LCPLicense *)license;
- (NSString *)valueForRight:(NSString *)rightIdentifier license:(LCPLicense *)license;
- (void)setValue:(NSString *)value forRight:(NSString *)rightIdentifier license:(LCPLicense *)license;

@end
