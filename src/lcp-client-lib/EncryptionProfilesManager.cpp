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