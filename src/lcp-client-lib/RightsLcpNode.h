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


#ifndef __RIGHTS_LCP_NODE_H__
#define __RIGHTS_LCP_NODE_H__

#include <mutex>
#include "LcpTypedefs.h"
#include "BaseLcpNode.h"
#include "public/IRights.h"
#include "public/IRightsService.h"
#include "IRightsManager.h"

namespace lcp
{
    struct RightsInfo
    {
        RightsInfo()
            : print(IRightsService::UNLIMITED)
            , copy(IRightsService::UNLIMITED)
            , tts(true)
        {
        }

        int print;
        int copy;
        bool tts;
        std::string start;
        std::string end;
        StringsMap valuesMap;
    };

    class RightsLcpNode : public BaseLcpNode, public IRights, public IRightsManager
    {
    public:
        // ILcpNode
        virtual void ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader);
        virtual Status VerifyNode(ILicense * license, IClientProvider * clientProvider, ICryptoProvider * cryptoProvider);
        virtual Status DecryptNode(ILicense * license, IKeyProvider * keyProvider, ICryptoProvider * cryptoProvider);

    public:
        // IRights
        virtual KvStringsIterator * Enumerate() const;
        virtual bool HasRightValue(const std::string & name) const;
        virtual bool GetRightValue(const std::string & name, std::string & value) const;

    public:
        // IRightsManager
        virtual bool CanUseRight(const std::string & name) const;
        virtual bool UseRight(const std::string & name);
        virtual bool UseRight(const std::string & name, int amount);
        virtual void SetRightValue(const std::string & name, const std::string & value);

    private:
        void SetRightValueInMap(const std::string & name, const std::string & value);
        void SetDefaultRightValuesInMap();
        bool DoesLicenseStart() const;
        bool DoesLicenseExpired() const;
        void FillRegisteredFields(const std::string & name, const rapidjson::Value & value);

    private:
        RightsInfo m_rights;
        mutable std::mutex m_rightsSync;

    };
}

#endif //__RIGHTS_LCP_NODE_H__
