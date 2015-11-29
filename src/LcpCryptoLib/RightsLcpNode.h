#ifndef __RIGHTS_LCP_NODE_H__
#define __RIGHTS_LCP_NODE_H__

#include "LcpUtils.h"
#include "BaseLcpNode.h"
#include "Public/IRights.h"
#include "IRightsManager.h"

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

    class RightsLcpNode : public BaseLcpNode, public IRights, public IRightsManager
    {
    public:
        // ILcpNode
        virtual void ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader);
        virtual Status VerifyNode(ILicense * license, IClientProvider * clientProvider, ICryptoProvider * cryptoProvider);

    public:
        // IRights
        virtual KvStringsIterator * Enumerate() const;
        virtual bool HasRightValue(const std::string & name) const;
        virtual bool GetRightValue(const std::string & name, std::string & value) const;

    public:
        // IRightsManager
        virtual bool HasRight(const std::string & name) const;
        virtual bool Consume(const std::string & name);
        virtual bool Consume(const std::string & name, int amount);
        virtual void SetRightValue(const std::string & name, const std::string & value);

    private:
        void SetRightValueInMap(const std::string & name, const std::string & value);
        bool DoesLicenseStart() const;
        bool DoesLicenseExpired() const;
        void FillRegisteredFields(const std::string & name, const rapidjson::Value & value);

    private:
        RightsInfo m_rights;
    };
}

#endif //__RIGHTS_LCP_NODE_H__
