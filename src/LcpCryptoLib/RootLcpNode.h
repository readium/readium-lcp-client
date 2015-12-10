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

#ifndef __ROOT_LCP_NODE_H__
#define __ROOT_LCP_NODE_H__

#include "BaseLcpNode.h"
#include "Public/ILicense.h"
#include "IKeyProvider.h"

namespace lcp
{
    struct RootInfo
    {
        std::string id;
        std::string content;
        std::string issued;
        std::string provider;
        std::string updated;
    };
    
    class RootLcpNode : public BaseLcpNode, public ILicense, public IKeyProvider
    {
    public:
        RootLcpNode(
            const std::string & licenseJson,
            const std::string & canonicalJson,
            ICrypto * crypto,
            ILinks * links,
            IUser * user,
            IRights * rights
            );

        void SetKeyProvider(std::unique_ptr<IKeyProvider> keyProvider);

    public:
        // ILcpNode
        virtual void ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader);
        virtual Status VerifyNode(ILicense * license, IClientProvider * clientProvider, ICryptoProvider * cryptoProvider);
        virtual Status DecryptNode(ILicense * license, IKeyProvider * keyProvider, ICryptoProvider * cryptoProvider);

    public:
        // ILicense
        virtual std::string Id() const;
        virtual std::string CanonicalContent() const;
        virtual std::string OriginalContent() const;
        virtual std::string Issued() const;
        virtual std::string Updated() const;
        virtual std::string Provider() const;

        virtual ICrypto * Crypto() const;
        virtual ILinks * Links() const;
        virtual IUser * User() const;
        virtual IRights * Rights() const;

        virtual bool Decrypted() const;

    public:
        virtual KeyType UserKey() const;
        virtual KeyType ContentKey() const;

    private:
        RootInfo m_rootInfo;
        ICrypto * m_crypto;
        ILinks * m_links;
        IUser * m_user;
        IRights * m_rights;
        std::string m_licenseJson;
        bool m_decrypted;
        std::unique_ptr<IKeyProvider> m_keyProvider;
    };
}

#endif //__ROOT_LCP_NODE_H__
