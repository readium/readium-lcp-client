// Copyright (c) 2016 Mantano
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


#if ENABLE_NET_PROVIDER

#import "LCPiOSNetProvider.h"

#import "../../../src/lcp-client-lib/public/lcp.h"

using namespace lcp;

@interface LCPiOSNetProvider : NSObject <NSURLSessionDataDelegate>
@property (strong, nonatomic) NSURLSession *session;
@property (strong, nonatomic) NSMutableDictionary *requests;
@property (strong, nonatomic) NSMutableDictionary *callbacks;
@end

@implementation LCPiOSNetProvider

- (instancetype)init
{
    self = [super init];
    if (self) {
        NSURLSessionConfiguration *config = [NSURLSessionConfiguration ephemeralSessionConfiguration];
        _session = [NSURLSession sessionWithConfiguration:config delegate:self delegateQueue:nil];
        
        _requests = [NSMutableDictionary dictionary];
        _callbacks = [NSMutableDictionary dictionary];
    }
    
    return self;
}

- (void)startDownloadRequest:(IDownloadRequest *)request callback:(INetProviderCallback *)callback
{
    NSString *urlString = [NSString stringWithUTF8String:request->Url().c_str()];
    NSURL *url = [NSURL URLWithString:urlString];
    if (url) {
        NSURLSessionDataTask *task = [self.session dataTaskWithURL:url];
        id identifier = @(task.taskIdentifier);
        self.requests[identifier] = [NSValue valueWithPointer:request];
        self.callbacks[identifier] = [NSValue valueWithPointer:callback];
        [task resume];
        
        callback->OnRequestStarted(request);
    }
}

- (void)URLSession:(NSURLSession *)session dataTask:(NSURLSessionDataTask *)dataTask didReceiveResponse:(NSURLResponse *)response completionHandler:(void (^)(NSURLSessionResponseDisposition))completionHandler
{
    lcp::IDownloadRequest *request;
    [self getRequest:&request callback:NULL forTask:dataTask];
    if (!request)
        return;
    
    NSString *filename = response.suggestedFilename;
    if (filename.length > 0) {
        request->SetSuggestedFileName([filename UTF8String]);
    }
    
    completionHandler(NSURLSessionResponseAllow);
}

- (void)URLSession:(NSURLSession *)session dataTask:(NSURLSessionDataTask *)task didReceiveData:(NSData *)data
{
    IDownloadRequest *request;
    INetProviderCallback *callback;
    [self getRequest:&request callback:&callback forTask:task];
    if (!request || !callback)
        return;
    
    if (request->Canceled()) {
        [task cancel];
        [self taskEnded:task];
        callback->OnRequestCanceled(request);
        
    } else {
        request->DestinationStream()->Write((const unsigned char *)data.bytes, data.length);
        
        float progress = -1;
        float received = task.countOfBytesReceived;
        float expected = task.countOfBytesExpectedToReceive;
        if (expected > 0) {
            progress = received / expected;
        }
        
        callback->OnRequestProgressed(request, progress);
    }
}

- (void)URLSession:(NSURLSession *)session task:(NSURLSessionTask *)task didCompleteWithError:(NSError *)error

{
    IDownloadRequest *request;
    INetProviderCallback *callback;
    [self getRequest:&request callback:&callback forTask:task];
    if (!request || !callback)
        return;
    
    if (request->Canceled()) {
        callback->OnRequestCanceled(request);
        
    } else {
        Status status = Status(StatusCode::ErrorCommonSuccess);
        if ([(NSHTTPURLResponse *)task.response statusCode] == 404) {
            status = Status(StatusCode::ErrorNetworkingRequestFailed);
        } else if (error) {
            status = Status(StatusCode::ErrorNetworkingRequestFailed);
        }
        
        callback->OnRequestEnded(request, status);
    }
    
    [self taskEnded:task];
}

- (void)getRequest:(IDownloadRequest **)request callback:(INetProviderCallback **)callback forTask:(NSURLSessionTask *)task
{
    id identifier = @(task.taskIdentifier);
    
    if (request != NULL) {
        *request = (IDownloadRequest *)[self.requests[identifier] pointerValue];
    }
    
    if (callback != NULL) {
        *callback = (INetProviderCallback *)[self.callbacks[identifier] pointerValue];
    }
}

- (void)taskEnded:(NSURLSessionTask *)task
{
    id identifier = @(task.taskIdentifier);
    [self.requests removeObjectForKey:identifier];
    [self.callbacks removeObjectForKey:identifier];
}

@end

namespace lcp
{
    iOSNetProvider::iOSNetProvider()
    {
        m_provider = [[LCPiOSNetProvider alloc] init];
    }
    
    void iOSNetProvider::StartDownloadRequest(IDownloadRequest *request, INetProviderCallback *callback)
    {
        [m_provider startDownloadRequest:request callback:callback];
    }
}

#endif //ENABLE_NET_PROVIDER
