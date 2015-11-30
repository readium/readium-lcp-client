//
//  Created by Mickaël Menu on 26/10/15.
//  Copyright © 2015 Mantano. All rights reserved.
//

#import "LCPiOSStorageProvider.h"

#import "third-parties/UICKeyChainStore/UICKeyChainStore.h"
#import <vector>

static NSString *const MNOLCPStorageProviderServiceTemplate = @"com.mantano.lcp.%@";

using namespace std;

namespace lcp
{
    class iOSKeyChainIterator : public KvStringsIterator
    {
    public:
        explicit iOSKeyChainIterator(UICKeyChainStore *keyChain) : m_index(0)
        {
            m_items = [keyChain allItems];
        }
        
        iOSKeyChainIterator() {}
        
        virtual void First()
        {
            m_index = 0;
        }
        
        virtual void Next()
        {
            m_index++;
        }
        
        virtual bool IsDone() const
        {
            return m_index >= [m_items count];
        }
        
        virtual const string & Current() const
        {
            if (IsDone()) {
                throw out_of_range("Iterator is out of range");
            }
 
#warning FIXME: this is temporary
            return std::move(std::string([m_items[m_index][@"value"] UTF8String]));
        }
        
        virtual std::string CurrentKey() const
        {
            if (IsDone()) {
                throw out_of_range("Iterator is out of range");
            }
 
            return [m_items[m_index][@"key"] UTF8String];
        }
        
    protected:
        NSArray *m_items;
        uint m_index;
    };
    
    string iOSStorageProvider::GetValue(const string &vaultId, const string &nativeKey)
    {
        UICKeyChainStore *keyChain = this->GetKeyChainOfVault(vaultId);
        NSString *key = this->GetStringFromNativeString(nativeKey);
        return [[keyChain stringForKey:key] UTF8String] ?: "";
    }
    
    void iOSStorageProvider::SetValue(const string &vaultId, const string &nativeKey, const string &nativeValue)
    {
        UICKeyChainStore *keyChain = this->GetKeyChainOfVault(vaultId);
        NSString *key = this->GetStringFromNativeString(nativeKey);
        NSString *value = this->GetStringFromNativeString(nativeValue);
        if (!keyChain || !key || !value)
            return;
        
        [keyChain setString:value forKey:key];
    }
    
    UICKeyChainStore *iOSStorageProvider::GetKeyChainOfVault(const string &vaultId)
    {
        UICKeyChainStore *keyChain;
        
        NSString *service = this->GetStringFromNativeString(vaultId);
        if ([service length] > 0) {
            service = [NSString stringWithFormat:MNOLCPStorageProviderServiceTemplate, service];
            keyChain = [UICKeyChainStore keyChainStoreWithService:service];
        }
        
        return keyChain;
    }
    
    KvStringsIterator *iOSStorageProvider::EnumerateVault(const std::string &vaultId)
    {
        UICKeyChainStore *keyChain = this->GetKeyChainOfVault(vaultId);
        return keyChain ? new iOSKeyChainIterator(keyChain) : nullptr;
    }
    
    NSString *iOSStorageProvider::GetStringFromNativeString(const string &nativeString)
    {
        NSString *string = [NSString stringWithUTF8String:nativeString.c_str()];
        return ([string length] > 0) ? string : nil;
    }
}
