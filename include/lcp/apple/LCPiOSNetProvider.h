//
//  Created by Mickaël Menu on 30/11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#import "../../../src/LcpCryptoLib/Public/INetProvider.h"
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
