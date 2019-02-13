// Licensed to the Readium Foundation under one or more contributor license agreements.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation and/or
//    other materials provided with the distribution.
// 3. Neither the name of the organization nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#import <Foundation/Foundation.h>

@interface StatusDocumentLink : NSObject {
    NSString* rel;
    NSString* href;
    NSString* type;
    BOOL templated;
    NSString* title;
    NSString* profile;
}

- (instancetype)init_:(NSString*) rel href:(NSString*) href type:(NSString*) type templated:(BOOL) templated title:(NSString*) title profile:(NSString*) profile;

@property (nonatomic, readonly) NSString* rel;
@property (nonatomic, readonly) NSString* href;
@property (nonatomic, readonly) NSString* type;
@property (nonatomic, readonly) BOOL templated;
@property (nonatomic, readonly) NSString* title;
@property (nonatomic, readonly) NSString* profile;

@end

@protocol DeviceIdManager

- (NSString*)getDeviceNAME;
- (NSString*)getDeviceID;
- (NSString*)checkDeviceID:(NSString*)key;
- (void)recordDeviceID:(NSString*)key;

@end

@class LCPStatusDocumentProcessing;

@protocol StatusDocumentProcessingListener

- (void)onStatusDocumentProcessingComplete:(LCPStatusDocumentProcessing*)lsdProcessing;

@end


@class LCPLicense;
@class LCPService;

typedef void (^DoneCallback)(bool);

@interface LCPStatusDocumentProcessing : NSObject

- (instancetype)init_:(LCPService *)service epubPath:(NSString *)epubPath license:(LCPLicense*)license deviceIdManager:(id<DeviceIdManager>)deviceIdManager;

- (void)start:(id<StatusDocumentProcessingListener>)listener;

- (bool) wasCancelled;
- (void)cancel;

-(bool)isInitialized;

-(bool)hasLicenseUpdatePending;

- (NSString *)identifier;
- (NSString *)message;

-(NSString *)status;
-(bool)isActive;

-(StatusDocumentLink *)licenseLink;

-(bool)hasRegisterLink;
-(StatusDocumentLink *)registerLink;

-(bool)hasRenewLink;
-(StatusDocumentLink *)renewLink;
-(void)doRenew:(DoneCallback)doneCallback_doRenew; //void(^)(bool)

-(bool)hasReturnLink;
-(StatusDocumentLink *)returnLink;
-(void)doReturn:(DoneCallback)doneCallback_doReturn; //void(^)(bool)

-(NSDate *)potentialRightsEnd;

- (NSDate *)statusUpdated;
- (NSDate *)licenseUpdated;

@end
