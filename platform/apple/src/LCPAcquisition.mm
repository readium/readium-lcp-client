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
#import "LCPAcquisition.h"

#import "IAcquistion.h"
#import "IAcquistionCallback.h"
#import "LCPError.h"

#if ENABLE_NET_PROVIDER_ACQUISITION

namespace lcp
{
    class AppleAcquisitionCallback : public IAcquisitionCallback
    {
    protected:
        LCPAcquisition *m_acquisition;
        
    public:
        AppleAcquisitionCallback(LCPAcquisition *acquisition) : m_acquisition(acquisition) {}
        
        virtual void OnAcquisitionStarted(IAcquisition *acquisition)
        {
            dispatch_async(dispatch_get_main_queue(), ^{
                id<LCPAcquisitionDelegate> delegate = m_acquisition.delegate;
                if ([delegate respondsToSelector:@selector(lcpAcquisitionDidStart:)]) {
                    [delegate lcpAcquisitionDidStart:m_acquisition];
                }
            });
        }
        
        virtual void OnAcquisitionProgressed(IAcquisition *acquisition, float progress)
        {
            dispatch_async(dispatch_get_main_queue(), ^{
                id<LCPAcquisitionDelegate> delegate = m_acquisition.delegate;
                if ([delegate respondsToSelector:@selector(lcpAcquisition:didProgress:)]) {
                    [delegate lcpAcquisition:m_acquisition didProgress:progress];
                }
            });
        }
        
        virtual void OnAcquisitionCanceled(IAcquisition *acquisition)
        {
            dispatch_async(dispatch_get_main_queue(), ^{
                id<LCPAcquisitionDelegate> delegate = m_acquisition.delegate;
                if ([delegate respondsToSelector:@selector(lcpAcquisitionDidCancel:)]) {
                    [delegate lcpAcquisitionDidCancel:m_acquisition];
                }
            });
        }
        
        virtual void OnAcquisitionEnded(IAcquisition *acquisition, Status status)
        {
            dispatch_async(dispatch_get_main_queue(), ^{
                id<LCPAcquisitionDelegate> delegate = m_acquisition.delegate;
                if ([delegate respondsToSelector:@selector(lcpAcquisition:didEnd:error:)]) {
                    BOOL success = Status::IsSuccess(status);
                    NSError *error = success ? nil : LCPErrorFromStatus(status);
                    [delegate lcpAcquisition:m_acquisition didEnd:success error:error];
                }
            });
        }
    };
}

@interface LCPAcquisition ()
@property (nonatomic) lcp::IAcquisitionCallback *callback;
@end

@implementation LCPAcquisition

- (instancetype)init
{
    return [self initWithAcquisition:nullptr];
}

- (instancetype)initWithAcquisition:(lcp::IAcquisition *)nativeAcquisition
{
    self = [super init];
    if (self) {
        _nativeAcquisition = nativeAcquisition;
    }
    
    return self;
}

- (void)dealloc
{
    delete _nativeAcquisition;
}

- (NSString *)publicationPath
{
    return [self stringFromNativeString:self.nativeAcquisition->PublicationPath()];
}

- (NSString *)suggestedFilename
{
    return [self stringFromNativeString:self.nativeAcquisition->SuggestedFileName()];
}

- (void)startWithDelegate:(id<LCPAcquisitionDelegate>)delegate
{
    self.delegate = delegate;
    self.callback = new lcp::AppleAcquisitionCallback(self);
    
    self.nativeAcquisition->Start(self.callback);
}

- (void)cancel
{
    self.nativeAcquisition->Cancel();
}

- (NSString *)stringFromNativeString:(std::string)nativeString
{
    return [NSString stringWithCString:nativeString.c_str() encoding:NSUTF8StringEncoding];
}

@end

#endif //ENABLE_NET_PROVIDER_ACQUISITION
