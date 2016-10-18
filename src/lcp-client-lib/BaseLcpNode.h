//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#ifndef __BASE_LCP_NODE_H__
#define __BASE_LCP_NODE_H__

#include <vector>
#include <memory>
#include <mutex>
#include <algorithm>
#include "ILcpNode.h"
#include "NonCopyable.h"

namespace lcp
{
    class ICryptoProvider;

    class BaseLcpNode : public ILcpNode, public NonCopyable
    {
    private:

        template <class _Function>
        inline //FORCE_INLINE
                _Function           ForEachChild(_Function __f) const
        {
            return std::for_each(m_childs.begin(), m_childs.end(), __f);
        }

    public:

        virtual void AddChildNode(ILcpNode* child)
        {
            std::unique_lock<std::mutex> locker(m_childsSync);
            std::unique_ptr<ILcpNode> uptr(child);
            m_childs.push_back(std::move(uptr));
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

                int v = m_childs.size();

                for (std::vector<std::unique_ptr<ILcpNode>>::const_iterator it = m_childs.begin(); it != m_childs.end(); ++it)
                {
                    //std::unique_ptr<ILcpNode> child = *it;
                    ILcpNode* child = (*it).get();
                    child->ParseNode(parentObject, reader);
                }
//
//                ForEachChild([&parentObject, reader](const std::unique_ptr<ILcpNode> & child)
//                             {
//                                 child->ParseNode(parentObject, reader);
//                             });
//
//                std::for_each(m_childs.begin(), m_childs.end(), [] (auto const& child) {
//                    bool breakpoint = true;
//                });

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
        std::vector<std::unique_ptr<ILcpNode>> m_childs;
        mutable std::mutex m_childsSync;
    };
}

#endif //__BASE_LCP_NODE_H__
