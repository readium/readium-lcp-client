//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#ifndef __RIGHTS_LCP_NODE_H__
#define __RIGHTS_LCP_NODE_H__

#include <mutex>
#include "LcpTypedefs.h"
#include "BaseLcpNode.h"
#include "public/IRights.h"
#include "public/IRightsService.h"
#include "IRightsManager.h"

namespace lcp
{
    struct RightsInfo
    {
        RightsInfo()
            : print(IRightsService::UNLIMITED)
            , copy(IRightsService::UNLIMITED)
            , tts(true)
        {
        }

        int print;
        int copy;
        bool tts;
        std::string start;
        std::string end;
        StringsMap valuesMap;
    };

    class RightsLcpNode : public BaseLcpNode, public IRights, public IRightsManager
    {
    public:
        // ILcpNode
        virtual void ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader);
        virtual Status VerifyNode(ILicense * license, IClientProvider * clientProvider, ICryptoProvider * cryptoProvider);
        virtual Status DecryptNode(ILicense * license, IKeyProvider * keyProvider, ICryptoProvider * cryptoProvider);

    public:
        // IRights
        virtual KvStringsIterator * Enumerate() const;
        virtual bool HasRightValue(const std::string & name) const;
        virtual bool GetRightValue(const std::string & name, std::string & value) const;

    public:
        // IRightsManager
        virtual bool CanUseRight(const std::string & name) const;
        virtual bool UseRight(const std::string & name);
        virtual bool UseRight(const std::string & name, int amount);
        virtual void SetRightValue(const std::string & name, const std::string & value);

    private:
        void SetRightValueInMap(const std::string & name, const std::string & value);
        void SetDefaultRightValuesInMap();
        bool DoesLicenseStart() const;
        bool DoesLicenseExpired() const;
        void FillRegisteredFields(const std::string & name, const rapidjson::Value & value);

    private:
        RightsInfo m_rights;
        mutable std::mutex m_rightsSync;

    };
}

#endif //__RIGHTS_LCP_NODE_H__
