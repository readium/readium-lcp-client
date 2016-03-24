//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

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
