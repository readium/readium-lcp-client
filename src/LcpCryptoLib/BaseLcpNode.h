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

#ifndef __BASE_LCP_NODE_H__
#define __BASE_LCP_NODE_H__

#include <vector>
#include <memory>
#include "ILcpNode.h"
#include "LcpUtils.h"

namespace lcp
{
    class ICryptoProvider;

    class BaseLcpNode : public ILcpNode
    {
    public:
        virtual void AddChildNode(std::unique_ptr<ILcpNode> child)
        {
            m_childs.push_back(std::move(child));
        }

        virtual bool IsLeaf() const
        {
            return m_childs.size() == 0;
        }

        virtual void ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader)
        {
            if (!IsLeaf())
            {
                for (auto it = m_childs.begin(); it != m_childs.end(); ++it)
                {
                    (*it)->ParseNode(parentObject, reader);
                }
            }
        }

        virtual Status DecryptNode(ILicense * license, IKeyProvider * keyProvider, ICryptoProvider * cryptoProvider)
        {
            if (!IsLeaf())
            {
                for (auto it = m_childs.begin(); it != m_childs.end(); ++it)
                {
                    Status res = (*it)->DecryptNode(license, keyProvider, cryptoProvider);
                    if (!Status::IsSuccess(res))
                        return res;
                }
            }
            return Status(StCodeCover::ErrorCommonSuccess);
        }

        virtual Status VerifyNode(ILicense * license, IClientProvider * clientProvider, ICryptoProvider * cryptoProvider)
        {
            if (!IsLeaf())
            {
                for (auto it = m_childs.begin(); it != m_childs.end(); ++it)
                {
                    Status res = (*it)->VerifyNode(license, clientProvider, cryptoProvider);
                    if (!Status::IsSuccess(res))
                        return res;
                }
            }
            return Status(StCodeCover::ErrorCommonSuccess);
        }

    protected:
        std::vector<std::unique_ptr<ILcpNode> > m_childs;
    };
}

#endif //__BASE_LCP_NODE_H__
