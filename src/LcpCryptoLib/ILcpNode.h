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

#ifndef __I_LCP_NODE_H__
#define __I_LCP_NODE_H__

#include <memory>
#include "Public/LcpStatus.h"
#include "rapidjson/document.h"

namespace lcp
{
    class JsonValueReader;
    class IKeyProvider;
    class ICryptoProvider;
    class ILicense;
    class IClientProvider;

    class ILcpNode
    {
    public:
        virtual void ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader) = 0;
        virtual Status VerifyNode(ILicense * license, IClientProvider * clientProvider, ICryptoProvider * cryptoProvider) = 0;
        virtual Status DecryptNode(ILicense * license, IKeyProvider * keyProvider, ICryptoProvider * cryptoProvider) = 0;
        virtual void AddChildNode(std::unique_ptr<ILcpNode> child) = 0;
        virtual bool IsLeaf() const = 0;
        virtual ~ILcpNode() {}
    };
}

#endif //__I_LCP_NODE_H__
