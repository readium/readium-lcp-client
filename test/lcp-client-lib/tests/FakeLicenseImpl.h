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



#ifndef __FAKE_LICENSE_IMPL_H__
#define __FAKE_LICENSE_IMPL_H__

#include <string>
#include "TestInfo.h"
#include "FakeCryptoImpl.h"
#include "public/lcp.h"
#include "RootLcpNode.h"

namespace lcptest
{
    class FakeLicenseImpl : public lcp::ILicense
    {
    public:
        explicit FakeLicenseImpl(FakeCryptoImpl * fakeCryptoImpl)
            : m_crypto(fakeCryptoImpl)
        {
            m_info.id = "df09ac25-a386-4c5c-b167-33ce4c36ca65";
            m_info.issued = "2015-11-24T13:32:39.905384033+01:00";
            m_info.updated = "2015-11-24T13:32:39.905384033+01:00";
            m_info.provider = "http://example.com";
            m_info.canonicalContent = TestCanonicalJson;
        }
        virtual std::string Id() const
        {
            return m_info.id;
        }
        virtual std::string CanonicalContent() const
        {
            return m_info.canonicalContent;
        }
        virtual std::string OriginalContent() const
        {
            return m_info.content;
        }

        void SetIssued(const std::string & value)
        {
            m_info.issued = value;
        }
        virtual std::string Issued() const
        {
            return m_info.issued;
        }

        void SetUpdated(const std::string & value)
        {
            m_info.updated = value;
        }
        virtual std::string Updated() const
        {
            return m_info.updated;
        }

        virtual std::string Provider() const
        {
            return m_info.provider;
        }
        virtual lcp::ICrypto * Crypto() const
        {
            return m_crypto;
        }
        virtual lcp::ILinks * Links() const
        {
            return nullptr;
        }
        virtual lcp::IUser * User() const
        {
            return nullptr;
        }
        virtual lcp::IRights * Rights() const
        {
            return nullptr;
        }
        virtual bool Decrypted() const
        {
            return false;
        }

    private:
        lcp::RootInfo m_info;
        FakeCryptoImpl * m_crypto;
    };
}

#endif //__FAKE_LICENSE_IMPL_H__
