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

#ifndef __ENCRYPTION_LCP_NODE_H__
#define __ENCRYPTION_LCP_NODE_H__

#include "BaseLcpNode.h"
#include "Public/ICrypto.h"

namespace lcp
{
    class IEncryptionProfile;
    class EncryptionProfilesManager;

    struct CryptoInfo
    {
        std::string encryptionProfile;
        std::string contentKey;
        std::string contentKeyAlgorithm;
        std::string userKeyHint;
        std::string userKeyAlgorithm;
        std::string userKeyCheck;
        std::string signature;
        std::string signatureAlgorithm;
        std::string signatureCertificate;
    };

    class CryptoLcpNode : public BaseLcpNode, public ICrypto
    {
    public:
        explicit CryptoLcpNode(EncryptionProfilesManager * encryptionProfilesManager);

        // ILcpNode
        void ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader);
        Status VerifyNode(ILicense * license, IClientProvider * clientProvider, ICryptoProvider * cryptoProvider);

        // ICrypto
        std::string EncryptionProfile() const;
        std::string ContentKey() const;
        std::string ContentKeyAlgorithm() const;
        std::string UserKeyHint() const;
        std::string UserKeyAlgorithm() const;
        std::string UserKeyCheck() const;
        std::string Signature() const;
        std::string SignatureAlgorithm() const;
        std::string SignatureCertificate() const;

    private:
        CryptoInfo m_cryptoInfo;
        IEncryptionProfile * m_encryptionProfile;
        EncryptionProfilesManager * m_encryptionProfilesManager;
    };
}
#endif //__ENCRYPTION_LCP_NODE_H__
