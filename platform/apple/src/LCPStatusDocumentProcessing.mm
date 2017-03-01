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

#import "LCPStatusDocumentProcessing.h"

//#import <platform/apple/src/lcp.h>
//#import "../../../src/lcp-client-lib/public/lcp.h"


#import "ILicense.h"
#import "LCPLicense.h"

#import "ILcpService.h"
#import "LCPService.h"

#import "ILinks.h"

#include <iostream>

using namespace lcp;


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


@interface StatusDocumentLink()
@end

@implementation StatusDocumentLink {
@private
}

@synthesize rel;
@synthesize href;
@synthesize type;
@synthesize templated;
@synthesize title;
@synthesize profile;


- (instancetype)init_:(NSString*) rel_ href:(NSString*) href_ type:(NSString*) type_ templated:(BOOL) templated_ title:(NSString*) title_ profile:(NSString*) profile_
{
    self = [super init];
    if (self) {
        
        rel = rel_ == nil ? @"" : rel_;
        href = href_ == nil ? @"" : href_;
        type = type_ == nil ? @"" : type_;
        templated = templated_;
        title = title_ == nil ? @"" : title_;
        profile = profile_ == nil ? @"" : profile_;
    }
    
    return self;
}
@end



@interface LCPStatusDocumentProcessing () <NSURLSessionDataDelegate> {
    
}
@end

@implementation LCPStatusDocumentProcessing {
@private
    LCPService * _service;
    NSString * _epubPath;
    LCPLicense * _license;
    id<DeviceIdManager> _deviceIDManager;
    
    bool _wasCancelled;
    id<StatusDocumentProcessingListener> _statusDocumentProcessingListener;
    
    NSString * _statusDocument_ID;
    NSString * _statusDocument_STATUS; // ready, active, revoked, returned, cancelled, expired
    NSString * _statusDocument_MESSAGE; // localized as per HTTP Accept-Language
    NSString * _statusDocument_UPDATED_LICENSE; //ISO 8601 time and date
    NSString * _statusDocument_UPDATED_STATUS; //ISO 8601 time and date
    StatusDocumentLink* _statusDocument_LINK_LICENSE; // HTTP GET URL, no-template
    StatusDocumentLink* _statusDocument_LINK_REGISTER;
    StatusDocumentLink* _statusDocument_LINK_RETURN;
    StatusDocumentLink* _statusDocument_LINK_RENEW;
    NSString * _statusDocument_POTENTIAL_RIGHTS_END; // ISO 8601 time and date

    NSMutableData *_data_TASK_DESCRIPTION_LCP_LSD_FETCH;
    NSMutableData *_data_TASK_DESCRIPTION_LCP_LSD_REGISTER;
    NSMutableData *_data_TASK_DESCRIPTION_LCP_FETCH;
    NSMutableData *_data_TASK_DESCRIPTION_LCP_LSD_RETURN;
    NSMutableData *_data_TASK_DESCRIPTION_LCP_LSD_RENEW;
    
    DoneCallback _doneCallback_registerDevice;
    DoneCallback _doneCallback_fetchAndInjectUpdatedLicense;
    DoneCallback _doneCallback_doReturn;
    DoneCallback _doneCallback_doRenew;
    
    bool _isInitialized;
}


NSString* TASK_DESCRIPTION_LCP_LSD_FETCH = @"LCP_LSD_FETCH";

NSString* TASK_DESCRIPTION_LCP_FETCH = @"LCP_FETCH";

NSString* TASK_DESCRIPTION_LCP_LSD_REGISTER = @"LCP_LSD_REGISTER";
NSString* TASK_DESCRIPTION_LCP_LSD_RETURN = @"LCP_LSD_RETURN";
NSString* TASK_DESCRIPTION_LCP_LSD_RENEW = @"LCP_LSD_RENEW";


- (instancetype)init_:(LCPService *)service epubPath:(NSString *)epubPath license:(LCPLicense*)license deviceIdManager:(id<DeviceIdManager>)deviceIdManager
{
    self = [super init];
    if (self) {
        _service = service;
        _epubPath = epubPath;
        _license = license;
        _deviceIDManager = deviceIdManager;
        
        _wasCancelled = false;
        _statusDocumentProcessingListener = nil;
        
        _statusDocument_ID = @"";
        _statusDocument_STATUS = @"";
        _statusDocument_MESSAGE = @"";
        _statusDocument_UPDATED_LICENSE = @"";
        _statusDocument_UPDATED_STATUS = @"";
        _statusDocument_LINK_LICENSE = nil;
        _statusDocument_LINK_REGISTER = nil;
        _statusDocument_LINK_RETURN = nil;
        _statusDocument_LINK_RENEW = nil;
        _statusDocument_POTENTIAL_RIGHTS_END = @"";
        
        _data_TASK_DESCRIPTION_LCP_LSD_FETCH = nil;
        _data_TASK_DESCRIPTION_LCP_LSD_REGISTER = nil;
        _data_TASK_DESCRIPTION_LCP_FETCH = nil;
    }
    
    return self;
}

- (bool) wasCancelled {
    return _wasCancelled;
}

- (void)cancel
{
    if (_wasCancelled) return;
    _wasCancelled = true;
    
    // forces re-check of LSD
    // (user cancellation does not mean byassing the initial LSD check,
    // unlike say: network failure)
    _license.nativeLicense->setStatusDocumentProcessingFlag(false);
    
    if (_statusDocumentProcessingListener != nil) {
        [_statusDocumentProcessingListener onStatusDocumentProcessingComplete:self];
    }
}



- (void)start:(id<StatusDocumentProcessingListener>)listener
{
    _statusDocumentProcessingListener = listener;
    
    std::string name("status");
    
    if (!_license.nativeLicense->Links()->Has(name)) {
        if (_statusDocumentProcessingListener != nil) {
            [_statusDocumentProcessingListener onStatusDocumentProcessingComplete:self];
        }
        return;
    }
    
    lcp::Link link;
    _license.nativeLicense->Links()->GetLink(name, link);
    
    NSString* urlString = [NSString stringWithUTF8String:link.href.c_str()];
    
    NSURL *url = [NSURL URLWithString:urlString];

    
    NSURLSessionConfiguration *config = [NSURLSessionConfiguration ephemeralSessionConfiguration];
    
    NSString * locale = [[NSLocale preferredLanguages] objectAtIndex:0];
    NSString* langCode = [NSString stringWithFormat:@"%@%@", locale, @",en-US;q=0.7,en;q=0.5"];

    NSURLSession * session = [NSURLSession sessionWithConfiguration:config delegate:self delegateQueue:nil]; //[NSOperationQueue mainQueue] // [[NSThread currentThread] isMainThread]
    
    NSMutableURLRequest* urlRequest = [[NSMutableURLRequest alloc] initWithURL:url];
    [urlRequest setHTTPMethod:@"GET"];
    [urlRequest setValue:langCode forHTTPHeaderField:@"Accept-Language"];
    // TODO: comment this in production! (this is only for testing a local HTTP server)
    //[urlRequest setValue:@"2s" forHTTPHeaderField:@"X-Add-Delay"];
    
    
    //NSURLSessionDataTask *task = [session dataTaskWithURL:url];
    NSURLSessionDataTask *task = [session dataTaskWithRequest:urlRequest];
    task.taskDescription = TASK_DESCRIPTION_LCP_LSD_FETCH;
    
    [task resume];
}



/////////////////////////////////////////
/////////////////////////////////////////
/////////////////////////////////////////


/////////////////////////////////////////
//NSURLSessionDataDelegate
- (void)URLSession:(NSURLSession *)session dataTask:(NSURLSessionDataTask *)dataTask
didReceiveResponse:(NSURLResponse *)response
 completionHandler:(void (^)(NSURLSessionResponseDisposition disposition))completionHandler
{
    if ([dataTask.taskDescription isEqualToString:TASK_DESCRIPTION_LCP_LSD_FETCH]) {
        
        _data_TASK_DESCRIPTION_LCP_LSD_FETCH = nil;
        
    } else if ([dataTask.taskDescription isEqualToString:TASK_DESCRIPTION_LCP_LSD_REGISTER]) {
        
        _data_TASK_DESCRIPTION_LCP_LSD_REGISTER = nil;
        
    } else if ([dataTask.taskDescription isEqualToString:TASK_DESCRIPTION_LCP_FETCH]) {
        
        _data_TASK_DESCRIPTION_LCP_FETCH = nil;
        
    } else if ([dataTask.taskDescription isEqualToString:TASK_DESCRIPTION_LCP_LSD_RETURN]) {
        
        _data_TASK_DESCRIPTION_LCP_LSD_RETURN = nil;
        
    } else if ([dataTask.taskDescription isEqualToString:TASK_DESCRIPTION_LCP_LSD_RENEW]) {
        
        _data_TASK_DESCRIPTION_LCP_LSD_RENEW = nil;
    }
    
    completionHandler(NSURLSessionResponseAllow);
}

/////////////////////////////////////////
//NSURLSessionDataDelegate
- (void)URLSession:(NSURLSession *)session dataTask:(NSURLSessionDataTask *)dataTask
    didReceiveData:(NSData *)data
{
    
    float progress = -1;
    float received = dataTask.countOfBytesReceived;
    float expected = dataTask.countOfBytesExpectedToReceive;
    if (expected > 0) {
        progress = received / expected;
    }
    
    
    if ([dataTask.taskDescription isEqualToString:TASK_DESCRIPTION_LCP_LSD_FETCH]) {
        
        if (_data_TASK_DESCRIPTION_LCP_LSD_FETCH == nil) {
            _data_TASK_DESCRIPTION_LCP_LSD_FETCH = [NSMutableData dataWithCapacity:(expected>0?expected:2048)];
        }
        [_data_TASK_DESCRIPTION_LCP_LSD_FETCH appendData:data];
        
        return;
    }
    
    if ([dataTask.taskDescription isEqualToString:TASK_DESCRIPTION_LCP_LSD_REGISTER]) {
        
        if (_data_TASK_DESCRIPTION_LCP_LSD_REGISTER == nil) {
            _data_TASK_DESCRIPTION_LCP_LSD_REGISTER = [NSMutableData dataWithCapacity:(expected>0?expected:2048)];
        }
        [_data_TASK_DESCRIPTION_LCP_LSD_REGISTER appendData:data];
        
        return;
    }
    
    if ([dataTask.taskDescription isEqualToString:TASK_DESCRIPTION_LCP_FETCH]) {
        
        if (_data_TASK_DESCRIPTION_LCP_FETCH == nil) {
            _data_TASK_DESCRIPTION_LCP_FETCH = [NSMutableData dataWithCapacity:(expected>0?expected:2048)];
        }
        [_data_TASK_DESCRIPTION_LCP_FETCH appendData:data];
        
        return;
    }
    
    if ([dataTask.taskDescription isEqualToString:TASK_DESCRIPTION_LCP_LSD_RENEW]) {
        
        if (_data_TASK_DESCRIPTION_LCP_LSD_RENEW == nil) {
            _data_TASK_DESCRIPTION_LCP_LSD_RENEW = [NSMutableData dataWithCapacity:(expected>0?expected:2048)];
        }
        [_data_TASK_DESCRIPTION_LCP_LSD_RENEW appendData:data];
        
        return;
    }
    
    if ([dataTask.taskDescription isEqualToString:TASK_DESCRIPTION_LCP_LSD_RETURN]) {
        
        if (_data_TASK_DESCRIPTION_LCP_LSD_RETURN == nil) {
            _data_TASK_DESCRIPTION_LCP_LSD_RETURN = [NSMutableData dataWithCapacity:(expected>0?expected:2048)];
        }
        [_data_TASK_DESCRIPTION_LCP_LSD_RETURN appendData:data];
        
        return;
    }
}

/////////////////////////////////////////
//NSURLSessionTaskDelegate
- (void)URLSession:(NSURLSession *)session task:(NSURLSessionTask *)task
didCompleteWithError:(nullable NSError *)error
{
    NSInteger code = [(NSHTTPURLResponse *)task.response statusCode];
    
    if ([task.taskDescription isEqualToString:TASK_DESCRIPTION_LCP_LSD_FETCH]) {
        
        if (error) {
            
            _data_TASK_DESCRIPTION_LCP_LSD_FETCH = nil;
            
            NSLog(@"%@", [NSString stringWithFormat:@"HTTP error (TASK_DESCRIPTION_LCP_LSD_FETCH) [%@] => (%li) ... %@ [%li]", [(NSHTTPURLResponse *)task.originalRequest URL], code, error.domain, error.code]);
            
            if (!_wasCancelled) {
                [_statusDocumentProcessingListener onStatusDocumentProcessingComplete:self];
            }
            
        } else if (code < 200 || code >= 300) {
            
            _data_TASK_DESCRIPTION_LCP_LSD_FETCH = nil;
            
            NSLog(@"%@", [NSString stringWithFormat:@"HTTP fail (TASK_DESCRIPTION_LCP_LSD_FETCH) [%@] => (%li)", [(NSHTTPURLResponse *)task.response URL], code]);
            
            if (!_wasCancelled) {
                [_statusDocumentProcessingListener onStatusDocumentProcessingComplete:self];
            }
            
        } else {
            
            try {
                NSString *json = [[NSString alloc] initWithData:_data_TASK_DESCRIPTION_LCP_LSD_FETCH encoding:NSUTF8StringEncoding];
                _data_TASK_DESCRIPTION_LCP_LSD_FETCH = nil;
                bool okay = [self parseStatusDocumentJson:json];
                if (okay) {
                    [self processStatusDocument];
                } else {
                    if (!_wasCancelled) {
                        [_statusDocumentProcessingListener onStatusDocumentProcessingComplete:self];
                    }
                }
            }
            catch (NSException *e) {
                
                NSLog(@"%@", [e reason]);
                
                if (!_wasCancelled) {
                    [_statusDocumentProcessingListener onStatusDocumentProcessingComplete:self];
                }
            }
            catch (std::exception& e) {
                
                auto msg = e.what();
                std::cout << msg << std::endl;
                
                if (!_wasCancelled) {
                    [_statusDocumentProcessingListener onStatusDocumentProcessingComplete:self];
                }
            }
            catch (...) {
                
                if (!_wasCancelled) {
                    [_statusDocumentProcessingListener onStatusDocumentProcessingComplete:self];
                }
            }
        }
        
        return;
    }
    
    if ([task.taskDescription isEqualToString:TASK_DESCRIPTION_LCP_LSD_REGISTER]) {
        
        if (error) {
            
            _data_TASK_DESCRIPTION_LCP_LSD_REGISTER = nil;
            
            NSLog(@"%@", [NSString stringWithFormat:@"HTTP error (TASK_DESCRIPTION_LCP_LSD_REGISTER) [%@] => (%li) ... %@ [%li]", [(NSHTTPURLResponse *)task.originalRequest URL], code, error.domain, error.code]);
            
            _doneCallback_registerDevice(false);
            
        } else if (code < 200 || code >= 300) {
            
            _data_TASK_DESCRIPTION_LCP_LSD_REGISTER = nil;
            
            NSLog(@"%@", [NSString stringWithFormat:@"HTTP fail (TASK_DESCRIPTION_LCP_LSD_REGISTER) [%@] => (%li)", [(NSHTTPURLResponse *)task.response URL], code]);
            
            _doneCallback_registerDevice(false);
            
        } else {
            
            try {
                NSString *json = [[NSString alloc] initWithData:_data_TASK_DESCRIPTION_LCP_LSD_REGISTER encoding:NSUTF8StringEncoding];
                _data_TASK_DESCRIPTION_LCP_LSD_REGISTER = nil;
                bool okay = [self parseStatusDocumentJson:json];
                if (okay && [_statusDocument_STATUS isEqualToString:@"active"]) {
                    [_deviceIDManager recordDeviceID:_statusDocument_ID];
                }
                
                _doneCallback_registerDevice(true);
            }
            catch (NSException *e) {
                
                NSLog(@"%@", [e reason]);
                
                _doneCallback_registerDevice(false);
            }
            catch (std::exception& e) {
                
                auto msg = e.what();
                std::cout << msg << std::endl;
                
                _doneCallback_registerDevice(false);
            }
            catch (...) {
                
                _doneCallback_registerDevice(false);
            }
        }
        
        return;
    }
    
    if ([task.taskDescription isEqualToString:TASK_DESCRIPTION_LCP_FETCH]) {
        
        if (error) {
            
            _data_TASK_DESCRIPTION_LCP_FETCH = nil;
            
            NSLog(@"%@", [NSString stringWithFormat:@"HTTP error (TASK_DESCRIPTION_LCP_LSD_FETCH) [%@] => (%li) ... %@ [%li]", [(NSHTTPURLResponse *)task.originalRequest URL], code, error.domain, error.code]);
            
            _doneCallback_fetchAndInjectUpdatedLicense(false);
            
        } else if (code < 200 || code >= 300) {
            
            _data_TASK_DESCRIPTION_LCP_FETCH = nil;
            
            NSLog(@"%@", [NSString stringWithFormat:@"HTTP fail (TASK_DESCRIPTION_LCP_LSD_FETCH) [%@] => (%li)", [(NSHTTPURLResponse *)task.response URL], code]);
            
            _doneCallback_fetchAndInjectUpdatedLicense(false);
            
        } else {
            
            try {
                NSString *json = [[NSString alloc] initWithData:_data_TASK_DESCRIPTION_LCP_FETCH encoding:NSUTF8StringEncoding];
                _data_TASK_DESCRIPTION_LCP_FETCH = nil;
                
                // new LCP license
                _service.nativeService->InjectLicense([_epubPath UTF8String], [json UTF8String]);
                
                // forces re-check of LSD, now with updated LCP timestamp
                _license.nativeLicense->setStatusDocumentProcessingFlag(false);

                _doneCallback_fetchAndInjectUpdatedLicense(true);
            }
            catch (NSException *e) {
                
                NSLog(@"%@", [e reason]);
                
                _doneCallback_fetchAndInjectUpdatedLicense(false);
            }
            catch (std::exception& e) {
                
                auto msg = e.what();
                std::cout << msg << std::endl;
                
                _doneCallback_fetchAndInjectUpdatedLicense(false);
            }
            catch (...) {
                
                _doneCallback_fetchAndInjectUpdatedLicense(false);
            }
        }
        
        return;
    }
    
    if ([task.taskDescription isEqualToString:TASK_DESCRIPTION_LCP_LSD_RENEW]) {
        
        if (error) {
            
            _data_TASK_DESCRIPTION_LCP_LSD_RENEW = nil;
            
            NSLog(@"%@", [NSString stringWithFormat:@"HTTP error (TASK_DESCRIPTION_LCP_LSD_RENEW) [%@] => (%li) ... %@ [%li]", [(NSHTTPURLResponse *)task.originalRequest URL], code, error.domain, error.code]);
            
            _doneCallback_doRenew(false);
            
        } else if (code < 200 || code >= 300) {
            
            _data_TASK_DESCRIPTION_LCP_LSD_RENEW = nil;
            
            NSLog(@"%@", [NSString stringWithFormat:@"HTTP fail (TASK_DESCRIPTION_LCP_LSD_RENEW) [%@] => (%li)", [(NSHTTPURLResponse *)task.response URL], code]);
            
            _doneCallback_doRenew(false);
            
        } else {
            
            try {
                _data_TASK_DESCRIPTION_LCP_LSD_RENEW = nil;
                
                // forces re-check of LSD, now with updated LCP timestamp
                _license.nativeLicense->setStatusDocumentProcessingFlag(false);
                
                _doneCallback_doRenew(true);
            }
            catch (NSException *e) {
                
                NSLog(@"%@", [e reason]);
                
                _doneCallback_doRenew(false);
            }
            catch (std::exception& e) {
                
                auto msg = e.what();
                std::cout << msg << std::endl;
                
                _doneCallback_doRenew(false);
            }
            catch (...) {
                
                _doneCallback_doRenew(false);
            }
        }
        
        return;
    }
    
    if ([task.taskDescription isEqualToString:TASK_DESCRIPTION_LCP_LSD_RETURN]) {
        
        if (error) {
            
            _data_TASK_DESCRIPTION_LCP_LSD_RETURN = nil;
            
            NSLog(@"%@", [NSString stringWithFormat:@"HTTP error (TASK_DESCRIPTION_LCP_LSD_RETURN) [%@] => (%li) ... %@ [%li]", [(NSHTTPURLResponse *)task.originalRequest URL], code, error.domain, error.code]);
            
            _doneCallback_doReturn(false);
            
        } else if (code < 200 || code >= 300) {
            
            _data_TASK_DESCRIPTION_LCP_LSD_RETURN = nil;
            
            NSLog(@"%@", [NSString stringWithFormat:@"HTTP fail (TASK_DESCRIPTION_LCP_LSD_RETURN) [%@] => (%li)", [(NSHTTPURLResponse *)task.response URL], code]);
            
            _doneCallback_doReturn(false);
            
        } else {
            
            try {
                _data_TASK_DESCRIPTION_LCP_LSD_RETURN = nil;
                
                // forces re-check of LSD, now with updated LCP timestamp
                _license.nativeLicense->setStatusDocumentProcessingFlag(false);
                
                _doneCallback_doReturn(true);
            }
            catch (NSException *e) {
                
                NSLog(@"%@", [e reason]);
                
                _doneCallback_doReturn(false);
            }
            catch (std::exception& e) {
                
                auto msg = e.what();
                std::cout << msg << std::endl;
                
                _doneCallback_doReturn(false);
            }
            catch (...) {
                
                _doneCallback_doReturn(false);
            }
        }
        
        return;
    }
}

/////////////////////////////////////////
/////////////////////////////////////////
/////////////////////////////////////////






- (bool)parseStatusDocumentJson:(NSString*)json
{
    _isInitialized = false;
    
    try {
        NSError *jsonError = nil;
        id rootJsonObj = [NSJSONSerialization JSONObjectWithData:[json dataUsingEncoding:NSUTF8StringEncoding] options:NSJSONReadingMutableContainers error:&jsonError];
        
        if (jsonError != nil) {
            return false;
        }
        
        if (rootJsonObj == nil) {
            return false;
        }
        
        BOOL okay = [NSJSONSerialization isValidJSONObject:rootJsonObj];
        if (!okay) {
            return false;
        }
        
        if (![rootJsonObj isKindOfClass:[NSDictionary class]]) {
            return false;
        }
        
        NSString* strTemp = nil;
        
        NSDictionary *rootJsonDict = (NSDictionary *)rootJsonObj;
        
        strTemp = [rootJsonDict valueForKey:@"id"];
        if (strTemp != nil) {
            _statusDocument_ID = strTemp;
        }
        
        strTemp = [rootJsonDict valueForKey:@"status"];
        if (strTemp != nil) {
            _statusDocument_STATUS = strTemp;
        }
        
        strTemp = [rootJsonDict valueForKey:@"message"];
        if (strTemp != nil) {
            _statusDocument_MESSAGE = strTemp;
        }
        
        NSDictionary *updatedJsonDict = [rootJsonDict valueForKey:@"updated"];
        
        strTemp = [updatedJsonDict valueForKey:@"license"];
        if (strTemp != nil) {
            _statusDocument_UPDATED_LICENSE = strTemp;
        }
        
        strTemp = [updatedJsonDict valueForKey:@"status"];
        if (strTemp != nil) {
            _statusDocument_UPDATED_STATUS = strTemp;
        }
        
        NSArray *linksJsonArray = [rootJsonDict valueForKey:@"links"];
        if (linksJsonArray != nil) {
            for (id linkJsonDict in linksJsonArray) {
                
                if (![linkJsonDict isKindOfClass:[NSDictionary class]]) {
                    return false;
                }
                
                NSString* rel = [linkJsonDict valueForKey:@"rel"];
                NSString* href = [linkJsonDict valueForKey:@"href"];
                NSString* type = [linkJsonDict valueForKey:@"type"];
                
                id templated_ = [linkJsonDict valueForKey:@"templated"];
                BOOL templated = NO;
                if ([templated_ isKindOfClass:[NSString class]]) {
                    if ([((NSString*)templated_) isEqualToString:@"true"]) {
                        templated = YES;
                    }
                } else if ([templated_ isKindOfClass:[NSNumber class]]) {
                    if (((NSNumber*)templated_) > 0) {
                        templated = YES;
                    }
                }
                
                NSString* title = [linkJsonDict valueForKey:@"title"];
                NSString* profile = [linkJsonDict valueForKey:@"profile"];
                
                StatusDocumentLink* link = [[StatusDocumentLink alloc] init_:rel href:href type:type templated:templated title:title profile:profile];
                
                if ([rel isEqualToString:@"license"]) {
                    _statusDocument_LINK_LICENSE = link;
                } else if ([rel isEqualToString:@"register"]) {
                    _statusDocument_LINK_REGISTER = link;
                } else if ([rel isEqualToString:@"return"]) {
                    _statusDocument_LINK_RETURN = link;
                } else if ([rel isEqualToString:@"renew"]) {
                    _statusDocument_LINK_RENEW = link;
                } else {
                    BOOL breakpoint = true;
                }
            }
        }

        
        NSDictionary* potentialRightsJsonDict = [rootJsonDict valueForKey:@"potential_rights"];
        if (potentialRightsJsonDict != nil) {
            strTemp = [potentialRightsJsonDict valueForKey:@"end"];
            if (strTemp != nil) {
                _statusDocument_POTENTIAL_RIGHTS_END = strTemp;
            }
        }
        
        NSArray* eventsJsonArray = [rootJsonDict valueForKey:@"events"];
        if (eventsJsonArray != nil) {
            for (id eventJsonDict in eventsJsonArray) {
                
                if (![eventJsonDict isKindOfClass:[NSDictionary class]]) {
                    break;
                }
                
                NSString* type = [eventJsonDict valueForKey:@"type"];
                NSString* name = [eventJsonDict valueForKey:@"name"];
                NSString* timestamp = [eventJsonDict valueForKey:@"timestamp"]; // ISO 8601 time and date
                NSString* id = [eventJsonDict valueForKey:@"id"];
            }
        }
        
        _isInitialized = true;
        return true;
    }
    catch (NSException *e) {
        
        NSLog(@"%@", [e reason]);
    }
    catch (std::exception& e) {
        
        auto msg = e.what();
        std::cout << msg << std::endl;
    }
    catch (...) {
        
    }
    
    return false;
}

-(bool)isInitialized {
    return _isInitialized;
}

-(bool)hasLicenseUpdatePending {
    if (_statusDocument_UPDATED_LICENSE == nil) {
        return false;
    }
    
    std::string t2([_statusDocument_UPDATED_LICENSE UTF8String]);
    
    std::string t1 = _license.nativeLicense->Updated();
    if (t1.length() == 0){
        t1 = _license.nativeLicense->Issued();
    }
    int compared = _service.nativeService->TimeStampCompare(t1, t2);
    
    BOOL licenseNeedsUpdating = (compared < 0); // license is older than status
    
    return (bool)licenseNeedsUpdating;
}

- (void)processStatusDocument
{
    if ([self hasLicenseUpdatePending]) {
        [self fetchAndInjectUpdatedLicense:^(bool done){
            if (!_wasCancelled) {
                [_statusDocumentProcessingListener onStatusDocumentProcessingComplete:self];
            }
        }];
        return;
    }
    
    
    if ([_statusDocument_STATUS isEqualToString:@"revoked"]
        || [_statusDocument_STATUS isEqualToString:@"returned"]
        || [_statusDocument_STATUS isEqualToString:@"cancelled"]
        || [_statusDocument_STATUS isEqualToString:@"expired"]
        ) {
        // Actually, this should never occur, because the LCP license should not even pass validation due to passed end date / expired timestamp
        
        if (!_wasCancelled) {
            [_statusDocumentProcessingListener onStatusDocumentProcessingComplete:self];
        }
        return;
    }
    
    
    [self checkLink_REGISTER:^(bool done){
        if (!_wasCancelled) {
            [_statusDocumentProcessingListener onStatusDocumentProcessingComplete:self];
        }
    }];
}

-(void)checkLink_REGISTER:(DoneCallback)doneCallback_checkLink_REGISTER //void(^)(bool)
{
    //checked in registerDevice(), and allows processing of renew and return interactions
    // if (_statusDocument_LINK_REGISTER == nil) {
    //    doneCallback_checkLink_REGISTER(false);
    //    return;
    //}
    
    [self registerDevice:^(bool done_registerDevice){
        
        doneCallback_checkLink_REGISTER(done_registerDevice);
        
        // application client side checks for return / renew interactions
//        
//        [self checkLink_RENEW:^(bool done_checkLink_RENEW){
//
//            if (done_checkLink_RENEW) {
//                doneCallback_checkLink_REGISTER(done_registerDevice);
//                return;
//            }
//            
//            [self checkLink_RETURN:^(bool done_checkLink_RETURN){
//                doneCallback_checkLink_REGISTER(done_registerDevice);
//            }];
//        }];
    }];
}

-(void)registerDevice:(DoneCallback)doneCallback_registerDevice //void(^)(bool)
{
    NSString* deviceNAME = [_deviceIDManager getDeviceNAME];
    NSString* deviceID = [_deviceIDManager getDeviceID];
    
    bool doRegister = false;
    if (_statusDocument_LINK_REGISTER == nil) {
        doRegister = false;
    } else if ([_statusDocument_STATUS isEqualToString:@"ready"]) {
        doRegister = true;
    } else if ([_statusDocument_STATUS isEqualToString:@"active"]) {
        
        NSString* deviceIDForStatusDoc = [_deviceIDManager checkDeviceID:_statusDocument_ID];
        
        if (deviceIDForStatusDoc == nil) {
            doRegister = true;
        } else if (![deviceIDForStatusDoc isEqualToString:deviceID]) { // this should really never happen ... but let's ensure anyway.
            doRegister = true;
        }
    }
    
    if (!doRegister) {
        doneCallback_registerDevice(false);
        return;
    }
    
    NSString* deviceID_escaped = [deviceID stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet URLQueryAllowedCharacterSet]];
    NSString* deviceNAME_escaped = [deviceNAME stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet URLQueryAllowedCharacterSet]];
    
    NSString* queryStr = [NSString stringWithFormat:@"id=%@&name=%@", deviceID_escaped, deviceNAME_escaped];
    
    NSString* urlString = _statusDocument_LINK_REGISTER.href;
    if (_statusDocument_LINK_REGISTER.templated) {
        
        urlString = [urlString stringByReplacingOccurrencesOfString:@"{?id,name}" withString:[NSString stringWithFormat:@"?%@", queryStr]]; // TODO: smarter regexp?
    }
    
    //urlString = [urlString stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding]; //NSASCIIStringEncoding
    //urlString = [urlString stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet URLQueryAllowedCharacterSet]];
    
    NSURL *url = [NSURL URLWithString:urlString];
    
    
    NSURLSessionConfiguration *config = [NSURLSessionConfiguration ephemeralSessionConfiguration];
    
    NSString * locale = [[NSLocale preferredLanguages] objectAtIndex:0];
    NSString* langCode = [NSString stringWithFormat:@"%@%@", locale, @",en-US;q=0.7,en;q=0.5"];
    
    NSURLSession * session = [NSURLSession sessionWithConfiguration:config delegate:self delegateQueue:nil]; //[NSOperationQueue mainQueue] // [[NSThread currentThread] isMainThread]
    
    NSMutableURLRequest* urlRequest = [[NSMutableURLRequest alloc] initWithURL:url];
    [urlRequest setHTTPMethod:@"POST"];
    [urlRequest setValue:langCode forHTTPHeaderField:@"Accept-Language"];
    // TODO: comment this in production! (this is only for testing a local HTTP server)
    //[urlRequest setValue:@"2s" forHTTPHeaderField:@"X-Add-Delay"];
    
    NSData *postData = [queryStr dataUsingEncoding:NSASCIIStringEncoding allowLossyConversion:YES];
    NSString *postLength = [NSString stringWithFormat:@"%lu", (unsigned long)[postData length]];
    [urlRequest setValue:postLength forHTTPHeaderField:@"Content-Length"];
    [urlRequest setValue:@"application/x-www-form-urlencoded" forHTTPHeaderField:@"Content-Type"];
    [urlRequest setHTTPBody:postData];
    
    //NSURLSessionDataTask *task = [session dataTaskWithURL:url];
    NSURLSessionDataTask *task = [session dataTaskWithRequest:urlRequest];
    task.taskDescription = TASK_DESCRIPTION_LCP_LSD_REGISTER;
    
    _doneCallback_registerDevice = doneCallback_registerDevice;
    
    [task resume];
}

-(void)fetchAndInjectUpdatedLicense:(DoneCallback)doneCallback_fetchAndInjectUpdatedLicense //void(^)(bool)
{
    if (_statusDocument_LINK_LICENSE == nil) {
        doneCallback_fetchAndInjectUpdatedLicense(false);
        return;
    }
    
    NSString* urlString = _statusDocument_LINK_LICENSE.href;
    
    NSURL *url = [NSURL URLWithString:urlString];
    
    
    NSURLSessionConfiguration *config = [NSURLSessionConfiguration ephemeralSessionConfiguration];
    
    NSString * locale = [[NSLocale preferredLanguages] objectAtIndex:0];
    NSString* langCode = [NSString stringWithFormat:@"%@%@", locale, @",en-US;q=0.7,en;q=0.5"];
    
    NSURLSession * session = [NSURLSession sessionWithConfiguration:config delegate:self delegateQueue:nil]; //[NSOperationQueue mainQueue] // [[NSThread currentThread] isMainThread]
    
    NSMutableURLRequest* urlRequest = [[NSMutableURLRequest alloc] initWithURL:url];
    [urlRequest setHTTPMethod:@"GET"];
    [urlRequest setValue:langCode forHTTPHeaderField:@"Accept-Language"];
    // TODO: comment this in production! (this is only for testing a local HTTP server)
    //[urlRequest setValue:@"2s" forHTTPHeaderField:@"X-Add-Delay"];
    
    //NSURLSessionDataTask *task = [session dataTaskWithURL:url];
    NSURLSessionDataTask *task = [session dataTaskWithRequest:urlRequest];
    task.taskDescription = TASK_DESCRIPTION_LCP_FETCH;
    
    _doneCallback_fetchAndInjectUpdatedLicense = doneCallback_fetchAndInjectUpdatedLicense;
    
    [task resume];
}


-(void)doRenew:(DoneCallback)doneCallback_doRenew //void(^)(bool)
{
    if (_statusDocument_LINK_RENEW == nil) {
        doneCallback_doRenew(false);
        return;
    }
    
    NSString* deviceNAME = [_deviceIDManager getDeviceNAME];
    NSString* deviceID = [_deviceIDManager getDeviceID];
    
    NSString* deviceID_escaped = [deviceID stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet URLQueryAllowedCharacterSet]];
    NSString* deviceNAME_escaped = [deviceNAME stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet URLQueryAllowedCharacterSet]];
    
    NSString* queryStr = [NSString stringWithFormat:@"id=%@&name=%@", deviceID_escaped, deviceNAME_escaped];
    
    NSString* urlString = _statusDocument_LINK_RENEW.href;
    if (_statusDocument_LINK_RENEW.templated) {
        
        urlString = [urlString stringByReplacingOccurrencesOfString:@"{?end,id,name}" withString:[NSString stringWithFormat:@"?%@", queryStr]]; // TODO: smarter regexp?
    }
    
    //urlString = [urlString stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding]; //NSASCIIStringEncoding
    //urlString = [urlString stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet URLQueryAllowedCharacterSet]];
    
    NSURL *url = [NSURL URLWithString:urlString];
    
    
    NSURLSessionConfiguration *config = [NSURLSessionConfiguration ephemeralSessionConfiguration];
    
    NSString * locale = [[NSLocale preferredLanguages] objectAtIndex:0];
    NSString* langCode = [NSString stringWithFormat:@"%@%@", locale, @",en-US;q=0.7,en;q=0.5"];
    
    NSURLSession * session = [NSURLSession sessionWithConfiguration:config delegate:self delegateQueue:nil]; //[NSOperationQueue mainQueue] // [[NSThread currentThread] isMainThread]
    
    NSMutableURLRequest* urlRequest = [[NSMutableURLRequest alloc] initWithURL:url];
    [urlRequest setHTTPMethod:@"PUT"];
    [urlRequest setValue:langCode forHTTPHeaderField:@"Accept-Language"];
    // TODO: comment this in production! (this is only for testing a local HTTP server)
    //[urlRequest setValue:@"2s" forHTTPHeaderField:@"X-Add-Delay"];
    
    NSData *postData = [queryStr dataUsingEncoding:NSASCIIStringEncoding allowLossyConversion:YES];
    NSString *postLength = [NSString stringWithFormat:@"%lu", (unsigned long)[postData length]];
    [urlRequest setValue:postLength forHTTPHeaderField:@"Content-Length"];
    [urlRequest setValue:@"application/x-www-form-urlencoded" forHTTPHeaderField:@"Content-Type"];
    [urlRequest setHTTPBody:postData];
    
    //NSURLSessionDataTask *task = [session dataTaskWithURL:url];
    NSURLSessionDataTask *task = [session dataTaskWithRequest:urlRequest];
    task.taskDescription = TASK_DESCRIPTION_LCP_LSD_RENEW;
    
    _doneCallback_doRenew = doneCallback_doRenew;
    
    [task resume];
}


-(void)doReturn:(DoneCallback)doneCallback_doReturn //void(^)(bool)
{
    if (_statusDocument_LINK_RETURN == nil) {
        doneCallback_doReturn(false);
        return;
    }
    
    NSString* deviceNAME = [_deviceIDManager getDeviceNAME];
    NSString* deviceID = [_deviceIDManager getDeviceID];
    
    NSString* deviceID_escaped = [deviceID stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet URLQueryAllowedCharacterSet]];
    NSString* deviceNAME_escaped = [deviceNAME stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet URLQueryAllowedCharacterSet]];
    
    NSString* queryStr = [NSString stringWithFormat:@"id=%@&name=%@", deviceID_escaped, deviceNAME_escaped];
    
    NSString* urlString = _statusDocument_LINK_RETURN.href;
    if (_statusDocument_LINK_RETURN.templated) {
        
        urlString = [urlString stringByReplacingOccurrencesOfString:@"{?id,name}" withString:[NSString stringWithFormat:@"?%@", queryStr]]; // TODO: smarter regexp?
    }
    
    //urlString = [urlString stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding]; //NSASCIIStringEncoding
    //urlString = [urlString stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet URLQueryAllowedCharacterSet]];
    
    NSURL *url = [NSURL URLWithString:urlString];
    
    
    NSURLSessionConfiguration *config = [NSURLSessionConfiguration ephemeralSessionConfiguration];
    
    NSString * locale = [[NSLocale preferredLanguages] objectAtIndex:0];
    NSString* langCode = [NSString stringWithFormat:@"%@%@", locale, @",en-US;q=0.7,en;q=0.5"];
    
    NSURLSession * session = [NSURLSession sessionWithConfiguration:config delegate:self delegateQueue:nil]; //[NSOperationQueue mainQueue] // [[NSThread currentThread] isMainThread]
    
    NSMutableURLRequest* urlRequest = [[NSMutableURLRequest alloc] initWithURL:url];
    [urlRequest setHTTPMethod:@"PUT"];
    [urlRequest setValue:langCode forHTTPHeaderField:@"Accept-Language"];
    // TODO: comment this in production! (this is only for testing a local HTTP server)
    //[urlRequest setValue:@"2s" forHTTPHeaderField:@"X-Add-Delay"];
    
    NSData *postData = [queryStr dataUsingEncoding:NSASCIIStringEncoding allowLossyConversion:YES];
    NSString *postLength = [NSString stringWithFormat:@"%lu", (unsigned long)[postData length]];
    [urlRequest setValue:postLength forHTTPHeaderField:@"Content-Length"];
    [urlRequest setValue:@"application/x-www-form-urlencoded" forHTTPHeaderField:@"Content-Type"];
    [urlRequest setHTTPBody:postData];
    
    //NSURLSessionDataTask *task = [session dataTaskWithURL:url];
    NSURLSessionDataTask *task = [session dataTaskWithRequest:urlRequest];
    task.taskDescription = TASK_DESCRIPTION_LCP_LSD_RETURN;
    
    _doneCallback_doReturn = doneCallback_doReturn;
    
    [task resume];
}


-(bool)isActive {
    return (_statusDocument_STATUS != nil && [_statusDocument_STATUS isEqualToString:@"active"]);
}

-(bool)hasRenewLink {
    return (_statusDocument_LINK_RENEW != nil);
}

-(bool)hasReturnLink {
    return (_statusDocument_LINK_RETURN != nil);
}

@end





//    dispatch_queue_t q_background = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_BACKGROUND, 0);
//    dispatch_async(q_background, ^{
//        dispatch_queue_t q_background_ = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_BACKGROUND, 0);
//        double delayInSeconds = 1.0;
//        dispatch_time_t popTime = dispatch_time(DISPATCH_TIME_NOW, delayInSeconds * NSEC_PER_SEC);
//        dispatch_after(popTime, q_background_, ^(void){
//
//        });
//    });

