#ifndef __ENCRYPTION_PROFILES_MANAGER_H__
#define __ENCRYPTION_PROFILES_MANAGER_H__

#include <map>
#include <memory>
#include "IEncryptionProfile.h"

namespace lcp
{
    class EncryptionProfilesManager
    {
    public:
        EncryptionProfilesManager();
        bool RegisterProfile(std::unique_ptr<IEncryptionProfile> profile);
        IEncryptionProfile * GetProfile(const std::string & name) const;

    private:
        typedef std::map<std::string, std::unique_ptr<IEncryptionProfile> > ProfilesMap;
        ProfilesMap m_profilesMap;
    };
}

#endif //__ENCRYPTION_PROFILES_MANAGER_H__
