#ifndef __RIGHTS_SERVICE_H__
#define __RIGHTS_SERVICE_H__

#include "Public/IRightsService.h"

namespace lcp
{
    class IRightsManager;
    class IStorageProvider;

    class RightsService : public IRightsService
    {
    public:
        explicit RightsService(IStorageProvider * storageProvider);
        void SyncRightsFromStorage(ILicense * license);

    public:
        // IRightsService
        virtual bool HasRight(ILicense * license, const std::string & rightId) const;
        virtual bool Consume(ILicense * license, const std::string & rightId);
        virtual bool Consume(ILicense * license, const std::string & rightId, int amount);
        virtual void SetValue(ILicense * license, const std::string & rightId, const std::string & value);
        virtual std::string GetValue(ILicense * license, const std::string & rightId) const;

    private:
        IRightsManager * PerformChecks(ILicense * license) const;
        std::string BuildStorageProviderRightsKeyPrefix(
            const std::string & providerId,
            const std::string & userId
            ) const;
        std::string BuildStorageProviderRightsKey(
            const std::string & providerId,
            const std::string & userId,
            const std::string & rightId
            ) const;
        std::string ExtractRightsKey(const std::string & storageProviderKey) const;

    private:
        IStorageProvider * m_storageProvider;
    };
}

#endif //__RIGHTS_SERVICE_H__
