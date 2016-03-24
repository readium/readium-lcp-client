//
//  Created by Mickaël Menu on 30/11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#import "../../../src/lcp-client-lib/public/INetProvider.h"
#import <Foundation/Foundation.h>

@class LCPiOSNetProvider;

namespace lcp
{
    class iOSNetProvider : public INetProvider
    {
    public:
        iOSNetProvider();
        
        virtual void StartDownloadRequest(IDownloadRequest *request, INetProviderCallback *callback);
        
    protected:
        LCPiOSNetProvider *m_provider;
    };
}
