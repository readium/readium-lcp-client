#ifndef __RIGHTS_LCP_NODE_H__
#define __RIGHTS_LCP_NODE_H__

#include "LcpUtils.h"
#include "BaseLcpNode.h"
#include "Public/IRights.h"

namespace lcp
{
    struct RightsInfo
    {
        RightsInfo()
            : print(UNLIMITED)
            , copy(UNLIMITED)
            , tts(true)
        {
        }

        int print;
        int copy;
        bool tts;
        std::string start;
        std::string end;
        StringsMap valuesMap;

        static int UNLIMITED;
    };

    class RightsLcpNode : public BaseLcpNode, public IRights
    {
    public:
        Status VerifyLicenseValidity() const;

        // ILcpNode
        virtual void ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader);

        // IRights
        virtual bool GetRightValue(const std::string & name, std::string & value) const;

    private:
        void FillRegisteredFields(const std::string & name, const rapidjson::Value & value);

    private:
        RightsInfo m_rights;
    };
}

#endif //__RIGHTS_LCP_NODE_H__
