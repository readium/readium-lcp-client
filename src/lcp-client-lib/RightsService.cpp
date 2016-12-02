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


#include <memory>
#include <sstream>
#include "RightsService.h"
#include "public/ILicense.h"
#include "public/IRights.h"
#include "public/IUser.h"
#include "public/IStorageProvider.h"
#include "IRightsManager.h"

namespace lcp
{
    /* static */ int IRightsService::UNLIMITED = -1;
    
    RightsService::RightsService(IStorageProvider * storageProvider, const std::string & unknownUserId)
        : m_storageProvider(storageProvider)
        , m_unknownUserId(unknownUserId)
    {
    }

    void RightsService::SyncRightsFromStorage(ILicense * license)
    {
        IRightsManager * rightsManager = this->PerformChecks(license);
        std::string keyPrefix = this->BuildStorageProviderRightsKeyPrefix(license);

        std::unique_ptr<KvStringsIterator> rightsIt(m_storageProvider->EnumerateVault(LicenseRightsVaultId));
        for (rightsIt->First(); !rightsIt->IsDone(); rightsIt->Next())
        {
            std::string storageKey = rightsIt->CurrentKey();
            if (storageKey.find(keyPrefix) != std::string::npos)
            {
                std::string rightId = this->ExtractRightsKey(storageKey);
                rightsManager->SetRightValue(rightId, rightsIt->Current());
            }
        }
    }

    bool RightsService::CanUseRight(ILicense * license, const std::string & rightId) const
    {
        IRightsManager * rightsManager = this->PerformChecks(license);
        return rightsManager->CanUseRight(rightId);
    }

    bool RightsService::UseRight(ILicense * license, const std::string & rightId)
    {
        return this->UseRight(license, rightId, 1);
    }

    bool RightsService::UseRight(ILicense * license, const std::string & rightId, int amount)
    {
        IRightsManager * rightsManager = this->PerformChecks(license);
        if (rightsManager->UseRight(rightId, amount))
        {
            std::string currentValue;
            license->Rights()->GetRightValue(rightId, currentValue);

            m_storageProvider->SetValue(
                LicenseRightsVaultId,
                this->BuildStorageProviderRightsKey(license, rightId),
                currentValue
                );
            return true;
        }
        return false;
    }

    void RightsService::SetValue(ILicense * license, const std::string & rightId, const std::string & value)
    {
        IRightsManager * rightsManager = this->PerformChecks(license);
        rightsManager->SetRightValue(rightId, value);

        m_storageProvider->SetValue(
            LicenseRightsVaultId,
            this->BuildStorageProviderRightsKey(license, rightId),
            value
            );
    }

    std::string RightsService::GetValue(ILicense * license, const std::string & rightId) const
    {
        std::string value;
        if (license->Rights()->GetRightValue(rightId, value))
        {
            return value;
        }
        else
        {
            return m_storageProvider->GetValue(
                LicenseRightsVaultId,
                this->BuildStorageProviderRightsKey(license, rightId)
                );
        }
    }

    std::string RightsService::BuildStorageProviderRightsKey(ILicense * license, const std::string & rightId) const
    {
        std::string userId = license->User()->Id();
        if (userId.empty())
        {
            userId = m_unknownUserId;
        }
        std::stringstream keyStream;
        keyStream << license->Provider() << "@" << userId << "@" << license->Id() << "@" << rightId;
        return keyStream.str();
    }

    std::string RightsService::BuildStorageProviderRightsKeyPrefix(ILicense * license) const
    {
        std::string userId = license->User()->Id();
        if (userId.empty())
        {
            userId = m_unknownUserId;
        }
        std::stringstream keyStream;
        keyStream << license->Provider() << "@" << userId << "@" << license->Id();
        return keyStream.str();
    }

    std::string RightsService::ExtractRightsKey(const std::string & storageProviderKey) const
    {
        size_t pos = storageProviderKey.find_last_of("@");
        if (pos == std::string::npos || pos + 1 == storageProviderKey.size())
        {
            throw std::runtime_error("Wrong storage provider rights key format: " + storageProviderKey);
        }
        return storageProviderKey.substr(pos + 1);
    }

    IRightsManager * RightsService::PerformChecks(ILicense * license) const
    {
        if (m_storageProvider == nullptr)
        {
            throw std::runtime_error("StorageProvider is nullptr");
        }
        IRightsManager * rightsManager = dynamic_cast<IRightsManager *>(license->Rights());
        if (rightsManager == nullptr)
        {
            throw std::logic_error("Can not cast IRights to IRightsManager");
        }
        return rightsManager;
    }
}