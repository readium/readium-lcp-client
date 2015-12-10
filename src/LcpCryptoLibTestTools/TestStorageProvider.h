//
//  Created by Artem Brazhnikov on 11/15.
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

#ifndef __TEST_STORAGE_PROVIDER_H__
#define __TEST_STORAGE_PROVIDER_H__

#include <map>
#include <exception>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "Public/lcp.h"

class TestStorageProvider : public lcp::IStorageProvider
{
    typedef std::map<std::string, std::string> StringsMap;

public:
    explicit TestStorageProvider(const std::string & fileName, bool newStorage = false)
        : m_flushed(false)
        , m_fileName(fileName)
    {
        if (!newStorage)
        {
            FILE* fp = fopen(m_fileName.c_str(), "r");
            if (fp == nullptr)
            {
                return;
            }

            char readBuffer[65536];
            rapidjson::FileReadStream fileReadStrm(fp, readBuffer, sizeof(readBuffer));
            rapidjson::Document doc;
            doc.ParseStream(fileReadStrm);

            if (doc.HasMember(lcp::UserKeysVaultId))
            {
                const rapidjson::Value & vaultUserKeys = doc[lcp::UserKeysVaultId];
                for (auto it = vaultUserKeys.MemberBegin(); it != vaultUserKeys.MemberEnd(); ++it)
                {
                    auto res = m_userKeysVault.insert(std::make_pair(
                        std::string(it->name.GetString(), it->name.GetStringLength()),
                        std::string(it->value.GetString(), it->value.GetStringLength()))
                        );
                    if (!res.second)
                    {
                        throw std::runtime_error("duplicate record");
                    }
                }
            }

            if (doc.HasMember(lcp::LicenseRightsVaultId))
            {
                const rapidjson::Value & vaultUserKeys = doc[lcp::LicenseRightsVaultId];
                for (auto it = vaultUserKeys.MemberBegin(); it != vaultUserKeys.MemberEnd(); ++it)
                {
                    auto res = m_licienseRightsVault.insert(std::make_pair(
                        std::string(it->name.GetString(), it->name.GetStringLength()),
                        std::string(it->value.GetString(), it->value.GetStringLength()))
                        );
                    if (!res.second)
                    {
                        throw std::runtime_error("duplicate record");
                    }
                }
            }

            fclose(fp);
        }
    }

    void Flush()
    {
        FILE* fp = fopen(m_fileName.c_str(), "w");
        if (fp == nullptr)
        {
            return;
        }

        rapidjson::Document doc;
        char writeBuffer[65536];
        rapidjson::FileWriteStream fileWriteStrm(fp, writeBuffer, sizeof(writeBuffer));

        doc.SetObject();
        rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
        
        rapidjson::Value vaultUserKeys(rapidjson::kObjectType);
        
        for (auto it = m_userKeysVault.begin(); it != m_userKeysVault.end(); ++it)
        {
            vaultUserKeys.AddMember(
                rapidjson::GenericStringRef<char>(it->first.c_str(), it->first.size()),
                rapidjson::GenericStringRef<char>(it->second.c_str(), it->second.size()),
                allocator
                );
        }
        doc.AddMember(rapidjson::GenericStringRef<char>(lcp::UserKeysVaultId, strlen(lcp::UserKeysVaultId)),
            vaultUserKeys, allocator
            );

        rapidjson::Value vaultLicense(rapidjson::kObjectType);
        for (auto it = m_licienseRightsVault.begin(); it != m_licienseRightsVault.end(); ++it)
        {
            vaultLicense.AddMember(
                rapidjson::GenericStringRef<char>(it->first.c_str(), it->first.size()),
                rapidjson::GenericStringRef<char>(it->second.c_str(), it->second.size()),
                allocator
                );
        }
        doc.AddMember(rapidjson::GenericStringRef<char>(lcp::LicenseRightsVaultId, strlen(lcp::LicenseRightsVaultId)),
            vaultLicense, allocator
            );

        rapidjson::Writer<rapidjson::FileWriteStream> writer(fileWriteStrm);
        doc.Accept(writer);

        fclose(fp);

        m_flushed = true;
    }

    ~TestStorageProvider()
    {
        if (!m_flushed)
        {
            this->Flush();
        }
    }

    virtual void SetValue(const std::string & vaultId, const std::string & key, const std::string & value)
    {
        StringsMap * vaultPtr = this->FindVault(vaultId);
        (*vaultPtr)[key] = value;
    }

    virtual std::string GetValue(const std::string & vaultId, const std::string & key)
    {
        StringsMap * vaultPtr = this->FindVault(vaultId);
        auto res = vaultPtr->find(key);
        if (res != vaultPtr->end())
            return res->second;
        return std::string();
    }

    virtual lcp::KvStringsIterator * EnumerateVault(const std::string & vaultId)
    {
        StringsMap * vaultPtr = this->FindVault(vaultId);
        return new lcp::MapIterator<std::string>(*vaultPtr);
    }

private:
    StringsMap * FindVault(const std::string & vaultId)
    {
        StringsMap * mapPtr = nullptr;
        if (vaultId == lcp::UserKeysVaultId)
        {
            mapPtr = &m_userKeysVault;
        }
        else if (vaultId == lcp::LicenseRightsVaultId)
        {
            mapPtr = &m_licienseRightsVault;
        }
        else
        {
            throw std::runtime_error("Vault was not found");
        }
        return mapPtr;
    }

private:
    StringsMap m_userKeysVault;
    StringsMap m_licienseRightsVault;
    std::string m_fileName;
    bool m_flushed;
};

#endif //__TEST_STORAGE_PROVIDER_H__
