//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#ifndef __FAKE_CRYPTO_IMPL_H__
#define __FAKE_CRYPTO_IMPL_H__

#include <string>
#include "TestInfo.h"
#include "public/lcp.h"
#include "CryptoLcpNode.h"

namespace lcptest
{
    class FakeCryptoImpl : public lcp::ICrypto
    {
    public:
        FakeCryptoImpl()
        {
            m_info.encryptionProfile = "http://readium.org/lcp/profile-1.0";
            m_info.contentKey = "7GMxrWNzUFpcwRNhclNECE3ZxvODsBt13N52zBoWdd6lQSNITVEJBXKxencovnPBBthBgE/6UE3sEmZYSfMrkQ==";
            m_info.contentKeyAlgorithm = "http://www.w3.org/2001/04/xmlenc#aes256-cbc";
            m_info.userKeyHint = "Enter your passphrase";
            m_info.userKeyAlgorithm = "http://www.w3.org/2001/04/xmlenc#sha256";
            m_info.userKeyCheck = "XW3MtvGZrWe74uhsrPRVki0eSDTTtM2x+g6YIA8BSObfWEeaLRRkoxGOgRuEcsnT2Fphhs5rB2xW8NI+mnHlIg==";
            m_info.signature = TestSignature;
            m_info.signatureAlgorithm = "http://www.w3.org/2001/04/xmldsig-more#rsa-sha256";
            m_info.signatureCertificate = TestCertificate;
        }

        void SetEncryptionProfile(const std::string & value)
        {
            m_info.encryptionProfile = value;
        }
        virtual std::string EncryptionProfile() const
        {
            return m_info.encryptionProfile;
        }

        virtual std::string ContentKey() const
        {
            return m_info.contentKey;
        }
        virtual std::string ContentKeyAlgorithm() const
        {
            return m_info.contentKeyAlgorithm;
        }
        virtual std::string UserKeyHint() const
        {
            return m_info.userKeyHint;
        }
        virtual std::string UserKeyAlgorithm() const
        {
            return m_info.userKeyAlgorithm;
        }
        virtual std::string UserKeyCheck() const
        {
            return m_info.userKeyCheck;
        }

        void SetSignature(const std::string & value)
        {
            m_info.signature = value;
        }
        virtual std::string Signature() const
        {
            return m_info.signature;
        }

        virtual std::string SignatureAlgorithm() const
        {
            return m_info.signatureAlgorithm;
        }

        void SetSignatureCertificate(const std::string & value)
        {
            m_info.signatureCertificate = value;
        }
        virtual std::string SignatureCertificate() const
        {
            return m_info.signatureCertificate;
        }

    private:
        lcp::CryptoInfo m_info;
    };
}

#endif //__FAKE_CRYPTO_IMPL_H__
