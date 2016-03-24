//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#include "EncryptionProfilesManager.h"
#include "Lcp1dot0EncryptionProfile.h"

namespace lcp
{
    EncryptionProfilesManager::EncryptionProfilesManager()
    {
        std::unique_ptr<IEncryptionProfile> lcp1dot0(new Lcp1dot0EncryptionProfile());
        m_profilesMap.insert(std::make_pair(lcp1dot0->Name(), std::move(lcp1dot0)));
    }

    bool EncryptionProfilesManager::RegisterProfile(std::unique_ptr<IEncryptionProfile> profile)
    {
        std::unique_lock<std::mutex> locker(m_profilesSync);
        auto res = m_profilesMap.insert(std::make_pair(profile->Name(), std::move(profile)));
        return res.second;
    }

    IEncryptionProfile * EncryptionProfilesManager::GetProfile(const std::string & name) const
    {
        std::unique_lock<std::mutex> locker(m_profilesSync);
        auto profileIt = m_profilesMap.find(name);
        if (profileIt != m_profilesMap.end())
        {
            return profileIt->second.get();
        }
        return nullptr;
    }
}