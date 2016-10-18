//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#ifndef __I_LCP_NODE_H__
#define __I_LCP_NODE_H__

#include <memory>
#include "public/LcpStatus.h"
#include "rapidjson/document.h"

namespace lcp
{
    class JsonValueReader;
    class IKeyProvider;
    class ICryptoProvider;
    class ILicense;
    class IClientProvider;

    class ILcpNode
    {
    public:
        virtual void ParseNode(const rapidjson::Value & parentObject, JsonValueReader * reader) = 0;
        virtual Status VerifyNode(ILicense * license, IClientProvider * clientProvider, ICryptoProvider * cryptoProvider) = 0;
        virtual Status DecryptNode(ILicense * license, IKeyProvider * keyProvider, ICryptoProvider * cryptoProvider) = 0;
        virtual void AddChildNode(ILcpNode* child) = 0;
        virtual bool IsLeaf() const = 0;
        virtual ~ILcpNode() {}
    };
}

#endif //__I_LCP_NODE_H__
