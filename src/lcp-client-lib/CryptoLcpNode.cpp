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


#include "rapidjson/document.h"
#include "CryptoLcpNode.h"
#include "JsonValueReader.h"
#include "IEncryptionProfile.h"
#include "CryptoAlgorithmInterfaces.h"
#include "EncryptionProfilesManager.h"

namespace lcp
{
    CryptoLcpNode::CryptoLcpNode(EncryptionProfilesManager * encryptionProfilesManager)
        : m_encryptionProfile(nullptr)
        , m_encryptionProfilesManager(encryptionProfilesManager)
    {
    }

    std::string CryptoLcpNode::EncryptionProfile() const
    {
        return m_cryptoInfo.encryptionProfile;
    }

    std::string CryptoLcpNode::ContentKey() const
    {
        return m_cryptoInfo.contentKey;
    }

    std::string CryptoLcpNode::ContentKeyAlgorithm() const
    {
        return m_cryptoInfo.contentKeyAlgorithm;
    }

    std::string CryptoLcpNode::UserKeyHint() const
    {
        return m_cryptoInfo.userKeyHint;
    }

    std::string CryptoLcpNode::UserKeyAlgorithm() const
    {
        return m_cryptoInfo.userKeyAlgorithm;
    }

    std::string CryptoLcpNode::UserKeyCheck() const
    {
        return m_cryptoInfo.userKeyCheck;
    }

    std::string CryptoLcpNode::Signature() const
    {
        return m_cryptoInfo.signature;
    }

    std::string CryptoLcpNode::SignatureAlgorithm() const
    {
        return m_cryptoInfo.signatureAlgorithm;
    }

    std::string CryptoLcpNode::SignatureCertificate() const
    {
        return m_cryptoInfo.signatureCertificate;
    }

#if ENABLE_GENERIC_JSON_NODE
    // noop
#else
    Status CryptoLcpNode::DecryptNode(ILicense * license, IKeyProvider * keyProvider, ICryptoProvider * cryptoProvider)
    {
        return Status(StatusCode::ErrorCommonSuccess);
    }
#endif //ENABLE_GENERIC_JSON_NODE

    Status CryptoLcpNode::VerifyNode(ILicense * license, IClientProvider * clientProvider, ICryptoProvider * cryptoProvider)
    {
#if ENABLE_PROFILE_NAMES
        m_encryptionProfile = m_encryptionProfilesManager->GetProfile(m_cryptoInfo.encryptionProfile);
        if (m_encryptionProfile == nullptr)
        {
            return Status(StatusCode::ErrorCommonEncryptionProfileNotFound, "ErrorCommonEncryptionProfileNotFound");
        }
#else
        m_encryptionProfile = m_encryptionProfilesManager->GetProfile();
#endif //ENABLE_PROFILE_NAMES

        if (m_encryptionProfile->ContentKeyAlgorithmCBC() != m_cryptoInfo.contentKeyAlgorithm && m_encryptionProfile->ContentKeyAlgorithmGCM() != m_cryptoInfo.contentKeyAlgorithm)
        {
            return Status(StatusCode::ErrorCommonAlgorithmMismatch, "ErrorCommonAlgorithmMismatch: content key algorithm mismatch");
        }
        if (m_encryptionProfile->UserKeyAlgorithm() != m_cryptoInfo.userKeyAlgorithm)
        {
            return Status(StatusCode::ErrorCommonAlgorithmMismatch, "ErrorCommonAlgorithmMismatch: user key algorithm mismatch");
        }
        if (m_encryptionProfile->SignatureAlgorithmRSA() != m_cryptoInfo.signatureAlgorithm && m_encryptionProfile->SignatureAlgorithmECDSA() != m_cryptoInfo.signatureAlgorithm)
        {
            return Status(StatusCode::ErrorCommonAlgorithmMismatch, "ErrorCommonAlgorithmMismatch: signature algorithm mismatch");
        }

#if ENABLE_GENERIC_JSON_NODE
        return BaseLcpNode::VerifyNode(license, clientProvider, cryptoProvider);
#else
        return Status(StatusCode::ErrorCommonSuccess);
#endif //ENABLE_GENERIC_JSON_NODE
    }

    void CryptoLcpNode::ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader)
    {
        const rapidjson::Value & encryptionObject = reader->ReadObjectCheck("encryption", parentObject);

        m_cryptoInfo.encryptionProfile = reader->ReadStringCheck("profile", encryptionObject);

        const rapidjson::Value & contentKeyObject = reader->ReadObjectCheck("content_key", encryptionObject);

        m_cryptoInfo.contentKey = reader->ReadStringCheck("encrypted_value", contentKeyObject);
        m_cryptoInfo.contentKeyAlgorithm = reader->ReadStringCheck("algorithm", contentKeyObject);

        const rapidjson::Value & userKeyObject = reader->ReadObjectCheck("user_key", encryptionObject);

        m_cryptoInfo.userKeyHint = reader->ReadStringCheck("text_hint", userKeyObject);
        m_cryptoInfo.userKeyAlgorithm = reader->ReadStringCheck("algorithm", userKeyObject);
        m_cryptoInfo.userKeyCheck = reader->ReadStringCheck("key_check", userKeyObject);

        // Child node
        const rapidjson::Value & signatureObject = reader->ReadObjectCheck("signature", parentObject);

        m_cryptoInfo.signatureAlgorithm = reader->ReadStringCheck("algorithm", signatureObject);
        m_cryptoInfo.signatureCertificate = reader->ReadStringCheck("certificate", signatureObject);
        m_cryptoInfo.signature = reader->ReadStringCheck("value", signatureObject);

#if ENABLE_GENERIC_JSON_NODE
        BaseLcpNode::ParseNode(encryptionObject, reader);
#else
        //child->ParseNode(encryptionObject, reader);
#endif //ENABLE_GENERIC_JSON_NODE
    }
}