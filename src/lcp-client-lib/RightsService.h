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


#ifndef __RIGHTS_SERVICE_H__
#define __RIGHTS_SERVICE_H__

#include "public/IRightsService.h"

namespace lcp
{
    class IRightsManager;
    class IStorageProvider;

    class RightsService : public IRightsService
    {
    public:
        explicit RightsService(IStorageProvider * storageProvider, const std::string & unknownUserId);
        void SyncRightsFromStorage(ILicense * license);

    public:
        // IRightsService
        virtual bool CanUseRight(ILicense * license, const std::string & rightId) const;
        virtual bool UseRight(ILicense * license, const std::string & rightId);
        virtual bool UseRight(ILicense * license, const std::string & rightId, int amount);
        virtual void SetValue(ILicense * license, const std::string & rightId, const std::string & value);
        virtual std::string GetValue(ILicense * license, const std::string & rightId) const;

    private:
        IRightsManager * PerformChecks(ILicense * license) const;
        std::string BuildStorageProviderRightsKeyPrefix(ILicense * license) const;
        std::string BuildStorageProviderRightsKey(ILicense * license, const std::string & rightId) const;
        std::string ExtractRightsKey(const std::string & storageProviderKey) const;

    private:
        IStorageProvider * m_storageProvider;
        std::string m_unknownUserId;
    };
}

#endif //__RIGHTS_SERVICE_H__
