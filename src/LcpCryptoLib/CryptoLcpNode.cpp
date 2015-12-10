//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//  Licensed under Gnu Affero General Public License Version 3 (provided, notwithstanding this notice,
//  Readium Foundation reserves the right to license this material under a different separate license,
//  and if you have done so, the terms of that separate license control and the following references
//  to GPL do not apply).
//
//  This program is free software: you can redistribute it and/or modify it under the terms of the GNU
//  Affero General Public License as published by the Free Software Foundation, either version 3 of
//  the License, or (at your option) any later version. You should have received a copy of the GNU
//  Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "rapidjson/document.h"
#include "CryptoLcpNode.h"
#include "JsonValueReader.h"
#include "LcpUtils.h"
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

    Status CryptoLcpNode::VerifyNode(ILicense * license, IClientProvider * clientProvider, ICryptoProvider * cryptoProvider)
    {
        m_encryptionProfile = m_encryptionProfilesManager->GetProfile(m_cryptoInfo.encryptionProfile);
        if (m_encryptionProfile == nullptr)
        {
            return Status(StatusCode::ErrorCommonEncryptionProfileNotFound);
        }

        if (m_encryptionProfile->ContentKeyAlgorithm() != m_cryptoInfo.contentKeyAlgorithm)
        {
            return Status(StatusCode::ErrorCommonAlgorithmMismatch, "content key algorithm mismatch");
        }
        if (m_encryptionProfile->UserKeyAlgorithm() != m_cryptoInfo.userKeyAlgorithm)
        {
            return Status(StatusCode::ErrorCommonAlgorithmMismatch, "user key algorithm mismatch");
        }
        if (m_encryptionProfile->SignatureAlgorithm() != m_cryptoInfo.signatureAlgorithm)
        {
            return Status(StatusCode::ErrorCommonAlgorithmMismatch, "signature algorithm mismatch");
        }
        return BaseLcpNode::VerifyNode(license, clientProvider, cryptoProvider);
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

        const rapidjson::Value & signatureObject = reader->ReadObjectCheck("signature", parentObject);//TODO: separate node

        m_cryptoInfo.signatureAlgorithm = reader->ReadStringCheck("algorithm", signatureObject);
        m_cryptoInfo.signatureCertificate = reader->ReadStringCheck("certificate", signatureObject);
        m_cryptoInfo.signature = reader->ReadStringCheck("value", signatureObject);

        BaseLcpNode::ParseNode(encryptionObject, reader);
    }
}