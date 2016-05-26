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
