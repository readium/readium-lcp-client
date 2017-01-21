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

#import "ILcpService.h"
#import "ILicense.h"
#import "LCPLicense.h"

using namespace lcp;

@interface LCPStatusDocumentProcessing () <NSURLSessionDataDelegate> {
    
}
@end

@implementation LCPStatusDocumentProcessing {
@private
    LCPService * _service;
    NSString * _path;
    LCPLicense * _license;
    id<DeviceIdManager> _deviceIdManager;
    
    bool _wasCancelled;
    id<StatusDocumentProcessingListener> _statusDocumentProcessingListener;
    
}

NSString* TASK_DESCRIPTION_LCP_LSD_FETCH = @"LCP_LSD_FETCH";

NSString* TASK_DESCRIPTION_LCP_FETCH = @"LCP_FETCH";

NSString* TASK_DESCRIPTION_LCP_LSD_REGISTER = @"LCP_LSD_REGISTER";
NSString* TASK_DESCRIPTION_LCP_LSD_RETURN = @"LCP_LSD_RETURN";
NSString* TASK_DESCRIPTION_LCP_LSD_RENEW = @"LCP_LSD_RENEW";


- (instancetype)init_:(LCPService *)service path:(NSString *)path license:(LCPLicense*)license deviceIdManager:(id<DeviceIdManager>)deviceIdManager
{
    self = [super init];
    if (self) {
        _service = service;
        _path = path;
        _license = license;
        _deviceIdManager = deviceIdManager;
        
        _wasCancelled = false;
        _statusDocumentProcessingListener = nil;
    }
    
    return self;
}

- (void)start:(id<StatusDocumentProcessingListener>)listener
{
    _statusDocumentProcessingListener = listener;
    
    NSURL *sourceUrl = [NSURL URLWithString:@"http://fake.domain.co.in/"];
    
    NSURLSessionConfiguration *config = [NSURLSessionConfiguration ephemeralSessionConfiguration];
    NSURLSession * session = [NSURLSession sessionWithConfiguration:config delegate:self delegateQueue:nil]; //[NSOperationQueue mainQueue] // [[NSThread currentThread] isMainThread]
    
    NSURLSessionDataTask *task = [session dataTaskWithURL:sourceUrl];
    task.taskDescription = TASK_DESCRIPTION_LCP_FETCH;
    //        id identifier = @(task.taskIdentifier);
    //        self.requests[identifier] = [NSValue valueWithPointer:request];
    //        self.callbacks[identifier] = [NSValue valueWithPointer:callback];
    [task resume];

    
//    dispatch_queue_t q_background = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_BACKGROUND, 0);
//    dispatch_async(q_background, ^{
//        dispatch_queue_t q_background_ = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_BACKGROUND, 0);
//        double delayInSeconds = 1.0;
//        dispatch_time_t popTime = dispatch_time(DISPATCH_TIME_NOW, delayInSeconds * NSEC_PER_SEC);
//        dispatch_after(popTime, q_background_, ^(void){
//            
//        });
//    });

}

- (bool) wasCancelled {
    return _wasCancelled;
}

- (void)cancel
{
    if (_wasCancelled) return;
    _wasCancelled = true;
    _license.nativeLicense->setStatusDocumentProcessingFlag(false);
    
    if (_statusDocumentProcessingListener != nil) {
        [_statusDocumentProcessingListener onStatusDocumentProcessingComplete:self];
    }
}



- (void)URLSession:(NSURLSession *)session dataTask:(NSURLSessionDataTask *)dataTask didReceiveResponse:(NSURLResponse *)response completionHandler:(void (^)(NSURLSessionResponseDisposition))completionHandler
{
    if (![dataTask.taskDescription isEqualToString:TASK_DESCRIPTION_LCP_FETCH]) {
        return;
    }
    
    completionHandler(NSURLSessionResponseAllow);
}

- (void)URLSession:(NSURLSession *)session dataTask:(NSURLSessionDataTask *)task didReceiveData:(NSData *)data
{
    if (![task.taskDescription isEqualToString:TASK_DESCRIPTION_LCP_FETCH]) {
        return;
    }
    
}

- (void)URLSession:(NSURLSession *)session task:(NSURLSessionTask *)task didCompleteWithError:(NSError *)error
{
    if (![task.taskDescription isEqualToString:TASK_DESCRIPTION_LCP_FETCH]) {
        return;
    }
    
    
    // TODO temp code, just to test immediate return and continue load EPUB
    //_license.nativeLicense->setStatusDocumentProcessingFlag(false);
    [_statusDocumentProcessingListener onStatusDocumentProcessingComplete:self];

}



@end
