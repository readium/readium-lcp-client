#ifndef __RIGHTS_LCP_NODE_H__
#define __RIGHTS_LCP_NODE_H__

#include <map>
#include "BaseLcpNode.h"
#include "IRights.h"

namespace lcp
{
    struct Rights
    {
        int print;
        int copy;
        bool tts;
        std::string start;
        std::string end;
        std::map<std::string, std::string> extendedRights;
    };

    class RightsLcpNode : public BaseLcpNode, public IRights
    {
    public:
        // ILcpNode
        virtual Status ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader);

    private:
        Rights m_rights;
    };
}

#endif //__RIGHTS_LCP_NODE_H__
