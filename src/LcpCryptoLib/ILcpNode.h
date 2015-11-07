#ifndef __I_LCP_NODE_H__
#define __I_LCP_NODE_H__

#include "LcpStatus.h"
#include "rapidjson/document.h"

// namespace rapidjson
// {
//     class Value;
// }

namespace lcp
{
    class JsonValueReader;

    class ILcpNode
    {
    public:
        virtual Status ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader) = 0;
        virtual void AddChildNode(ILcpNode * child) = 0;
        virtual bool IsLeaf() const = 0;
        virtual ~ILcpNode() {}
    };
}

#endif //__I_LCP_NODE_H__
