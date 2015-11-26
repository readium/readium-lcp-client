//
//  Created by Mickaël Menu on 26/10/15.
//  Copyright © 2015 Mantano. All rights reserved.
//

#import "LcpiOSStorageProvider.h"

#import "third-parties/UICKeyChainStore/UICKeyChainStore.h"

static NSString *const MNOLCPStorageProviderServiceTemplate = @"com.mantano.lcp.%@";

namespace lcp
{
    typedef std::map<std::string, std::string> StringsMap;
    
    std::string LcpiOSStorageProvider::GetValue(const std::string &vaultId, const std::string &nativeKey)
    {
        UICKeyChainStore *keychain = this->GetKeyChainOfVault(vaultId);
        NSString *key = this->GetStringFromNativeString(nativeKey);
        return [[keychain stringForKey:key] UTF8String] ?: "";
    }
    
    void LcpiOSStorageProvider::SetValue(const std::string &vaultId, const std::string &nativeKey, const std::string &nativeValue)
    {
        UICKeyChainStore *keychain = this->GetKeyChainOfVault(vaultId);
        NSString *key = this->GetStringFromNativeString(nativeKey);
        NSString *value = this->GetStringFromNativeString(nativeValue);
        if (!keychain || !key || !value)
            return;
        
        [keychain setString:value forKey:key];
    }
    
    UICKeyChainStore *LcpiOSStorageProvider::GetKeyChainOfVault(const std::string &vaultId)
    {
        UICKeyChainStore *keychain;
        
        NSString *service = this->GetStringFromNativeString(vaultId);
        if ([service length] > 0) {
            service = [NSString stringWithFormat:MNOLCPStorageProviderServiceTemplate, service];
            keychain = [UICKeyChainStore keyChainStoreWithService:service];
        }
        
        return keychain;
    }
    
    MapIterator<std::string> *LcpiOSStorageProvider::EnumerateVault(const std::string &vaultId)
    {
        StringsMap vault;
        UICKeyChainStore *keychain = this->GetKeyChainOfVault(vaultId);
        if (keychain) {
            for (NSDictionary *item in [keychain allItems]) {
                NSString *key = item[@"key"];
                NSString *value = item[@"value"];
                if ([key isKindOfClass:NSString.class] && [value isKindOfClass:NSString.class]) {
                    vault[[key UTF8String]] = [value UTF8String];
                }
            }
        }
        
        return new MapIterator<std::string>(vault);
    }
    
    NSString *LcpiOSStorageProvider::GetStringFromNativeString(const std::string &nativeString)
    {
        NSString *string = [NSString stringWithUTF8String:nativeString.c_str()];
        return ([string length] > 0) ? string : nil;
    }
}
