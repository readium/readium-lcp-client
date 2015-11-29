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
    class NSArrayValueIterator : public IValueIterator<string>
    {
    public:
        explicit NSArrayValueIterator(NSArray *array) : m_index(0)
        {
            m_values->resize([array count]);
            for (NSString *value in array) {
                m_values->push_back([value UTF8String]);
            }
        }
        
        ~NSArrayValueIterator() {}
        
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
            return m_index >= m_values->size();
        }
        
        virtual const string & Current() const
        {
            if (IsDone()) {
                throw out_of_range("Iterator is out of range");
            }
 
            return m_values->at(m_index);
        }
        
    protected:
        vector<string> *m_values;
        uint m_index;
    };
    
    string iOSStorageProvider::GetValue(const string &vaultId, const string &nativeKey)
    {
        UICKeyChainStore *keychain = this->GetKeyChainOfVault(vaultId);
        NSString *key = this->GetStringFromNativeString(nativeKey);
        return [[keychain stringForKey:key] UTF8String] ?: "";
    }
    
    void iOSStorageProvider::SetValue(const string &vaultId, const string &nativeKey, const string &nativeValue)
    {
        UICKeyChainStore *keychain = this->GetKeyChainOfVault(vaultId);
        NSString *key = this->GetStringFromNativeString(nativeKey);
        NSString *value = this->GetStringFromNativeString(nativeValue);
        if (!keychain || !key || !value)
            return;
        
        [keychain setString:value forKey:key];
    }
    
    UICKeyChainStore *iOSStorageProvider::GetKeyChainOfVault(const string &vaultId)
    {
        UICKeyChainStore *keychain;
        
        NSString *service = this->GetStringFromNativeString(vaultId);
        if ([service length] > 0) {
            service = [NSString stringWithFormat:MNOLCPStorageProviderServiceTemplate, service];
            keychain = [UICKeyChainStore keyChainStoreWithService:service];
        }
        
        return keychain;
    }
    
    IValueIterator<string> *iOSStorageProvider::EnumerateVault(const string & vaultId)
    {
        UICKeyChainStore *keychain = this->GetKeyChainOfVault(vaultId);
        return keychain ? new NSArrayValueIterator([keychain allItems]) : nullptr;
    }
    
    NSString *iOSStorageProvider::GetStringFromNativeString(const string &nativeString)
    {
        NSString *string = [NSString stringWithUTF8String:nativeString.c_str()];
        return ([string length] > 0) ? string : nil;
    }
}
