//
//  Created by Mickaël Menu on 30/11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//

#import "LCPiOSNetProvider.h"

#import "../../../include/lcp/lcp.h"


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

- (void)startDownloadRequest:(lcp::IDownloadRequest *)request callback:(lcp::INetProviderCallback *)callback
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

- (void)URLSession:(NSURLSession *)session dataTask:(NSURLSessionDataTask *)dataTask didReceiveResponse:(NSURLResponse *)response completionHandler:(void (^)(NSURLSessionResponseDisposition))completionHandler
{
    lcp::IDownloadRequest *request;
    [self getRequest:&request callback:NULL forTask:dataTask];
    if (!request)
        return;
    
#warning TODO: set suggested filename
}

- (void)URLSession:(NSURLSession *)session downloadTask:(NSURLSessionDownloadTask *)task didWriteData:(int64_t)bytesWritten totalBytesWritten:(int64_t)received totalBytesExpectedToWrite:(int64_t)expected
{
    lcp::IDownloadRequest *request;
    lcp::INetProviderCallback *callback;
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
    lcp::IDownloadRequest *request;
    lcp::INetProviderCallback *callback;
    [self getRequest:&request callback:&callback forTask:task];
    if (!request || !callback)
        return;
    
    NSString *toPath = [NSString stringWithUTF8String:((lcp::IFile *)request->DestinationFile())->GetPath().c_str()];
    
    NSError *error;
    [[NSFileManager defaultManager] removeItemAtPath:toPath error:NULL];
    if (![[NSFileManager defaultManager] moveItemAtPath:[location path] toPath:toPath error:&error]) {
        [self taskEnded:task];
        callback->OnRequestEnded(request, lcp::Status(lcp::StCodeCover::ErrorNetworkingRequestFailed, "Can't move the downloaded file to destination path"));
    }
}

// Used only if we use a DataTask instead of a DownloadTask, by using the IWritableFile

//- (void)URLSession:(NSURLSession *)session dataTask:(NSURLSessionDataTask *)dataTask didReceiveData:(NSData *)data
//{
//    lcp::IDownloadRequest *request;
//    lcp::INetProviderCallback *callback;
//    [self getRequest:&request callback:&callback forTask:dataTask];
//    if (!request || !callback)
//        return;
//    
//    if (request->Canceled()) {
//        [dataTask cancel];
//        [self taskEnded:dataTask];
//        callback->OnRequestCanceled(request);
//        
//        
//    } else {
//        float received = dataTask.countOfBytesReceived;
//        float expected = dataTask.countOfBytesExpectedToReceive;
//        float progress = -1;
//        if (expected > 0) {
//            progress = received / expected;
//        }
//        
//        request->DestinationFile()->Write(received, data.bytes, data.length);
//        callback->OnRequestProgressed(request, progress);
//    }
//}

- (void)URLSession:(NSURLSession *)session task:(NSURLSessionTask *)task didCompleteWithError:(NSError *)error

{
    lcp::IDownloadRequest *request;
    lcp::INetProviderCallback *callback;
    [self getRequest:&request callback:&callback forTask:task];
    if (!request || !callback)
        return;
    
    if (request->Canceled()) {
        callback->OnRequestCanceled(request);
        
    } else {
        lcp::Status status = lcp::Status(lcp::StCodeCover::ErrorCommonSuccess);
        if ([(NSHTTPURLResponse *)task.response statusCode] == 404) {
            status = lcp::Status(lcp::StCodeCover::ErrorNetworkingRequestFailed);
        } else if (error) {
            status = lcp::Status(lcp::StCodeCover::ErrorNetworkingRequestFailed);
        }
        
        callback->OnRequestEnded(request, status);
    }
    
    [self taskEnded:task];
}

- (void)getRequest:(lcp::IDownloadRequest **)request callback:(lcp::INetProviderCallback **)callback forTask:(NSURLSessionTask *)task
{
    id identifier = @(task.taskIdentifier);
    
    if (request != NULL) {
        *request = (lcp::IDownloadRequest *)[self.requests[identifier] pointerValue];
    }
    
    if (callback != NULL) {
        *callback = (lcp::INetProviderCallback *)[self.callbacks[identifier] pointerValue];
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
