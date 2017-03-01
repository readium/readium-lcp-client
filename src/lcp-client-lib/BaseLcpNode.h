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
#if ENABLE_GENERIC_JSON_NODE
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
#endif //ENABLE_GENERIC_JSON_NODE
    };
}

#endif //__BASE_LCP_NODE_H__
