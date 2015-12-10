//
//  Created by Mickaël Menu on 30/11/15.
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

#import "LCPiOSNetProvider.h"

#import "../../../include/lcp/lcp.h"

using namespace lcp;

@interface LCPiOSNetProvider : NSObject <NSURLSessionDownloadDelegate>
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
        NSURLSessionDownloadTask *task = [self.session downloadTaskWithURL:url];
        id identifier = @(task.taskIdentifier);
        self.requests[identifier] = [NSValue valueWithPointer:request];
        self.callbacks[identifier] = [NSValue valueWithPointer:callback];
        [task resume];
        
        callback->OnRequestStarted(request);
    }
}

- (void)URLSession:(NSURLSession *)session downloadTask:(NSURLSessionDownloadTask *)task didWriteData:(int64_t)bytesWritten totalBytesWritten:(int64_t)received totalBytesExpectedToWrite:(int64_t)expected
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
        float progress = -1;
        if (expected > 0) {
            progress = (float)received / (float)expected;
        }
        
        callback->OnRequestProgressed(request, progress);
    }
}

- (void)URLSession:(NSURLSession *)session downloadTask:(NSURLSessionDownloadTask *)task didFinishDownloadingToURL:(NSURL *)location
{
    IDownloadRequest *request;
    INetProviderCallback *callback;
    [self getRequest:&request callback:&callback forTask:task];
    if (!request || !callback)
        return;
    
    NSString *filename = task.response.suggestedFilename;
    if (filename.length > 0) {
        request->SetSuggestedFileName([filename UTF8String]);
    }
    
    NSString *toPath = [NSString stringWithUTF8String:request->DestinationPath().c_str()];
    
    NSError *error;
    [[NSFileManager defaultManager] removeItemAtPath:toPath error:NULL];
    if (![[NSFileManager defaultManager] moveItemAtPath:[location path] toPath:toPath error:&error]) {
        [self taskEnded:task];
        callback->OnRequestEnded(request, Status(StatusCode::ErrorNetworkingRequestFailed, "Can't move the downloaded file to destination path"));
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
        StartDownloadRequestAsync(request, callback);
    }
    
    void iOSNetProvider::StartDownloadRequestAsync(IDownloadRequest *request, INetProviderCallback *callback)
    {
        [m_provider startDownloadRequest:request callback:callback];
    }
}
