#ifndef __RIGHTS_LCP_NODE_H__
#define __RIGHTS_LCP_NODE_H__

#include <map>
#include "BaseLcpNode.h"
#include "IRights.h"

namespace lcp
{
    struct Rights
    {
        int Print;
        int Copy;
        bool Tts;
        std::string Start;
        std::string End;
        std::map<std::string, std::string> ExtendedRights;

        static const char * PRINT;
        static const char * COPY;
        static const char * TTS;
        static const char * START;
        static const char * END;
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
