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

#ifndef __USER_LCP_NODE_H__
#define __USER_LCP_NODE_H__

#include "LcpUtils.h"
#include "BaseLcpNode.h"
#include "Public/IUser.h"

namespace lcp
{
    class ILicense;

    struct UserInfo
    {
        std::string id;
        std::string email;
        std::string name;
        std::vector<std::string> encrypted;
        StringsMap valuesMap;
    };

    class UserLcpNode : public BaseLcpNode, public IUser
    {
    public:
        // ILcpNode
        virtual void ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader);
        virtual Status DecryptNode(ILicense * license, IKeyProvider * keyProvider, ICryptoProvider * cryptoProvider);

    public:
        // IUser
        virtual std::string Id() const;
        virtual std::string Email() const;
        virtual std::string Name() const;
        virtual bool HasUserValue(const std::string & name) const;
        virtual bool GetUserValue(const std::string & name, std::string & value) const;
        virtual KvStringsIterator * Enumerate() const;

    private:
        void FillRegisteredFields(const std::string & name, const std::string & value);

    private:
        UserInfo m_userInfo;
    };
}

#endif //__USER_LCP_NODE_H__