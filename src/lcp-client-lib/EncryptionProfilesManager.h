//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#ifndef __ENCRYPTION_PROFILES_MANAGER_H__
#define __ENCRYPTION_PROFILES_MANAGER_H__

#include <map>
#include <memory>
#include <mutex>
#include "IEncryptionProfile.h"
#include "NonCopyable.h"

namespace lcp
{
    class EncryptionProfilesManager : public NonCopyable
    {
    public:
        EncryptionProfilesManager();
        bool RegisterProfile(std::unique_ptr<IEncryptionProfile> profile);
        IEncryptionProfile * GetProfile(const std::string & name) const;

    private:
        typedef std::map<std::string, std::unique_ptr<IEncryptionProfile> > ProfilesMap;
        ProfilesMap m_profilesMap;
        mutable std::mutex m_profilesSync;
    };
}

#endif //__ENCRYPTION_PROFILES_MANAGER_H__
