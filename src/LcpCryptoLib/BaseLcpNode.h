#ifndef __BASE_LCP_NODE_H__
#define __BASE_LCP_NODE_H__

#include <vector>
#include "ILcpNode.h"

namespace lcp
{
    class BaseLcpNode : public ILcpNode
    {
    public:
        virtual void AddChildNode(ILcpNode * child)
        {
            m_childs.push_back(child);
        }
        virtual bool IsLeaf() const
        {
            return m_childs.size() == 0;
        }

    protected:
        std::vector<ILcpNode *> m_childs;
    };
}

#endif //__BASE_LCP_NODE_H__
