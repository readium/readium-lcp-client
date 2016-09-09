//
//  Created by Mickaël Menu on 26/10/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#import "LCPiOSStorageProvider.h"

#import "third-parties/UICKeyChainStore/UICKeyChainStore.h"
#import <vector>

static NSString *const MNOLCPStorageProviderServiceTemplate = @"org.readium.lcp.%@";

using namespace std;

namespace lcp
{
    class iOSKeyChainIterator : public KvStringsIterator
    {
    public:
        explicit iOSKeyChainIterator(UICKeyChainStore *keyChain) : m_index(0)
        {
            m_keys = [keyChain allKeys];
            
            for (NSString *key in m_keys) {
                NSString* str = [keyChain stringForKey:key];
                if (str != nil)
                    m_values.push_back(std::string([str UTF8String]));
                    //m_values.push_back(std::string("DUMMY"));
                else
                    m_values.push_back(std::string("KEYCHAIN ACCESS REFUSED"));
            }
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
            return m_index >= m_values.size();
        }
        
        virtual const string & Current() const
        {
            if (IsDone()) {
                throw out_of_range("Iterator is out of range");
            }
 
            return m_values.at(m_index);
        }
        
        virtual std::string CurrentKey() const
        {
            if (IsDone()) {
                throw out_of_range("Iterator is out of range");
            }
 
            return [m_keys[m_index] UTF8String];
        }
        
    protected:
        vector<string> m_values;
        NSArray *m_keys;
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
    
    void iOSStorageProvider::EraseVault(const std::string &vaultId)
    {
        UICKeyChainStore *keyChain = this->GetKeyChainOfVault(vaultId);
        [keyChain removeAllItems];
    }
    
    NSString *iOSStorageProvider::GetStringFromNativeString(const string &nativeString)
    {
        NSString *string = [NSString stringWithUTF8String:nativeString.c_str()];
        return ([string length] > 0) ? string : nil;
    }
}
