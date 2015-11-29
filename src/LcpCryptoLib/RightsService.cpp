#include <memory>
#include "RightsService.h"
#include "Public/ILicense.h"
#include "Public/IRights.h"
#include "Public/IUser.h"
#include "Public/IStorageProvider.h"
#include "IRightsManager.h"

namespace lcp
{
    RightsService::RightsService(IStorageProvider * storageProvider)
        : m_storageProvider(storageProvider)
    {
    }

    void RightsService::SyncRightsFromStorage(ILicense * license)
    {
        IRightsManager * rightsManager = this->PerformChecks(license);
        std::string keyPrefix = this->BuildStorageProviderRightsKeyPrefix(license->Provider(), license->User()->Id());

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

    bool RightsService::HasRight(ILicense * license, const std::string & rightId) const
    {
        IRightsManager * rightsManager = this->PerformChecks(license);
        return rightsManager->HasRight(rightId);
    }

    bool RightsService::Consume(ILicense * license, const std::string & rightId)
    {
        return this->Consume(license, rightId, 1);
    }

    bool RightsService::Consume(ILicense * license, const std::string & rightId, int amount)
    {
        IRightsManager * rightsManager = this->PerformChecks(license);
        if (rightsManager->Consume(rightId, amount))
        {
            std::string currentValue;
            license->Rights()->GetRightValue(rightId, currentValue);

            m_storageProvider->SetValue(
                LicenseRightsVaultId,
                this->BuildStorageProviderRightsKey(
                    license->Provider(), license->User()->Id(), rightId
                    ),
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
            this->BuildStorageProviderRightsKey(
                license->Provider(), license->User()->Id(), rightId
                ),
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
        return std::string();

        /* Read from storage
        if (m_storageProvider == nullptr)
        {
            throw std::runtime_error("StorageProvider is nullptr");
        }
        return m_storageProvider->GetValue(
            LicenseRightsVaultId,
            this->BuildStorageProviderRightsKey(
                license->Provider(), license->User()->Id(), rightId
                )
            );*/
    }

    std::string RightsService::BuildStorageProviderRightsKey(
        const std::string & providerId,
        const std::string & userId,
        const std::string & rightId
        ) const
    {
        return providerId + "@" + userId + "@" + rightId;
    }

    std::string RightsService::BuildStorageProviderRightsKeyPrefix(
        const std::string & providerId,
        const std::string & userId
        ) const
    {
        return providerId + "@" + userId;
    }

    std::string RightsService::ExtractRightsKey(const std::string & storageProviderKey) const
    {
        size_t pos = storageProviderKey.find_last_of("@");
        if (pos == std::string::npos)
        {
            throw std::runtime_error("Wrong storage provider rights key format: " + storageProviderKey);
        }
        return storageProviderKey.substr(pos);
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