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
