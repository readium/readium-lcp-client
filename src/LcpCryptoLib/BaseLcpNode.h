#ifndef __BASE_LCP_NODE_H__
#define __BASE_LCP_NODE_H__

#include <vector>
#include <memory>
#include "ILcpNode.h"

namespace lcp
{
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

    protected:
        std::vector<std::unique_ptr<ILcpNode> > m_childs;
    };
}

#endif //__BASE_LCP_NODE_H__
