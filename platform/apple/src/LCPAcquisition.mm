//
//  Created by Mickaël Menu on 02/11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//

#import "LCPAcquisition.h"

#import "IAcquistion.h"
#import "IAcquistionCallback.h"
#import "LCPError.h"

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
            id<LCPAcquisitionDelegate> delegate = m_acquisition.delegate;
            if ([delegate respondsToSelector:@selector(lcpAcquisitionDidStart:)]) {
                [delegate lcpAcquisitionDidStart:m_acquisition];
            }
        }
        
        virtual void OnAcquisitionProgressed(IAcquisition *acquisition, float progress)
        {
            id<LCPAcquisitionDelegate> delegate = m_acquisition.delegate;
            if ([delegate respondsToSelector:@selector(lcpAcquisition:didProgress:)]) {
                [delegate lcpAcquisition:m_acquisition didProgress:progress];
            }
        }
        
        virtual void OnAcquisitionCanceled(IAcquisition *acquisition)
        {
            id<LCPAcquisitionDelegate> delegate = m_acquisition.delegate;
            if ([delegate respondsToSelector:@selector(lcpAcquisitionDidCancel:)]) {
                [delegate lcpAcquisitionDidCancel:m_acquisition];
            }
        }
        
        virtual void OnAcquisitionEnded(IAcquisition *acquisition, Status status)
        {
            id<LCPAcquisitionDelegate> delegate = m_acquisition.delegate;
            if ([delegate respondsToSelector:@selector(lcpAcquisition:didEnd:error:)]) {
                BOOL success = Status::IsSuccess(status);
                NSError *error = success ? nil : LCPErrorFromStatus(status);
                [delegate lcpAcquisition:m_acquisition didEnd:success error:error];
            }
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
