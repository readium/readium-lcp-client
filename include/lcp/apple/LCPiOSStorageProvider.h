//
//  Created by Mickaël Menu on 26/10/15.
//  Copyright © 2015 Mantano. All rights reserved.
//

#import "../../../src/LcpCryptoLib/Public/IStorageProvider.h"
#import <Foundation/Foundation.h>

@class UICKeyChainStore;

namespace lcp
{
    class iOSStorageProvider : public IStorageProvider
    {
    public:
        virtual std::string GetValue(const std::string &vaultId, const std::string &key);
        virtual void SetValue(const std::string &vaultId, const std::string &key, const std::string &value);
        virtual IValueIterator<std::string> *EnumerateVault(const std::string & vaultId);
        
    private:
        NSString *GetStringFromNativeString(const std::string &nativeString);
        UICKeyChainStore *GetKeyChainOfVault(const std::string &vaultId);
    };
}
