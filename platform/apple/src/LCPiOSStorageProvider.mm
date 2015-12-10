//
//  Created by Mickaël Menu on 26/10/15.
//  Copyright © 2015 Mantano. All rights reserved.
//
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//  Licensed under Gnu Affero General Public License Version 3 (provided, notwithstanding this notice,
//  Readium Foundation reserves the right to license this material under a different separate license,
//  and if you have done so, the terms of that separate license control and the following references
//  to GPL do not apply).
//
//  This program is free software: you can redistribute it and/or modify it under the terms of the GNU
//  Affero General Public License as published by the Free Software Foundation, either version 3 of
//  the License, or (at your option) any later version. You should have received a copy of the GNU
//  Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
            m_keys = [keyChain allKeys];
            
            for (NSString *key in m_keys) {
                m_values.push_back(std::string([[keyChain stringForKey:key] UTF8String]));
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
