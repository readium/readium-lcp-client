//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//  Licensed under Gnu Affero General Public License Version 3 (provided, notwithstanding this notice,
//  Readium Foundation reserves the right to license this material under a different separate license,
//  and if you have done so, the terms of that separate license control and the following references
//  to GPL do not apply).
//
//  This program is free software: you can redistribute it and/or modify it under the terms of the GNU
//  Affero General Public License as published by the Free Software Foundation, either version 3 of
//  the License, or (at your option) any later version. You should have received a copy of the GNU
//  Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef __RIGHTS_LCP_NODE_H__
#define __RIGHTS_LCP_NODE_H__

#include "LcpUtils.h"
#include "BaseLcpNode.h"
#include "Public/IRights.h"
#include "Public/IRightsService.h"
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
    };
}

#endif //__RIGHTS_LCP_NODE_H__
