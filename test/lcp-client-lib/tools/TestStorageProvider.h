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


#ifndef __TEST_STORAGE_PROVIDER_H__
#define __TEST_STORAGE_PROVIDER_H__

#include <map>
#include <exception>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "public/lcp.h"
#include "ContainerIterator.h"

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
