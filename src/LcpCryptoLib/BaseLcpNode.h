//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#ifndef __BASE_LCP_NODE_H__
#define __BASE_LCP_NODE_H__

#include <vector>
#include <memory>
#include <mutex>
#include "ILcpNode.h"
#include "NonCopyable.h"

namespace lcp
{
    class ICryptoProvider;

    class BaseLcpNode : public ILcpNode, public NonCopyable
    {
    public:
        virtual void AddChildNode(std::unique_ptr<ILcpNode> child)
        {
            std::unique_lock<std::mutex> locker(m_childsSync);
            m_childs.push_back(std::move(child));
        }

        virtual bool IsLeaf() const
        {
            std::unique_lock<std::mutex> locker(m_childsSync);
            return m_childs.size() == 0;
        }

        virtual void ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader)
        {
            if (!IsLeaf())
            {
                std::unique_lock<std::mutex> locker(m_childsSync);

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
                std::unique_lock<std::mutex> locker(m_childsSync);

                for (auto it = m_childs.begin(); it != m_childs.end(); ++it)
                {
                    Status res = (*it)->DecryptNode(license, keyProvider, cryptoProvider);
                    if (!Status::IsSuccess(res))
                        return res;
                }
            }
            return Status(StatusCode::ErrorCommonSuccess);
        }

        virtual Status VerifyNode(ILicense * license, IClientProvider * clientProvider, ICryptoProvider * cryptoProvider)
        {
            if (!IsLeaf())
            {
                std::unique_lock<std::mutex> locker(m_childsSync);

                for (auto it = m_childs.begin(); it != m_childs.end(); ++it)
                {
                    Status res = (*it)->VerifyNode(license, clientProvider, cryptoProvider);
                    if (!Status::IsSuccess(res))
                        return res;
                }
            }
            return Status(StatusCode::ErrorCommonSuccess);
        }

    protected:
        std::vector<std::unique_ptr<ILcpNode> > m_childs;
        mutable std::mutex m_childsSync;
    };
}

#endif //__BASE_LCP_NODE_H__
