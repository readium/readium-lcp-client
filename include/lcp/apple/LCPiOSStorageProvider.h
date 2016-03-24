//
//  Created by Mickaël Menu on 26/10/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#import "../../../src/lcp-client-lib/public/IStorageProvider.h"
#import <Foundation/Foundation.h>

@class UICKeyChainStore;

namespace lcp
{
    class iOSStorageProvider : public IStorageProvider
    {
    public:
        virtual std::string GetValue(const std::string &vaultId, const std::string &key);
        virtual void SetValue(const std::string &vaultId, const std::string &key, const std::string &value);
        virtual KvStringsIterator *EnumerateVault(const std::string &vaultId);
        void EraseVault(const std::string &vaultId);
        
    private:
        NSString *GetStringFromNativeString(const std::string &nativeString);
        UICKeyChainStore *GetKeyChainOfVault(const std::string &vaultId);
    };
}
