//
//  Created by Mickaël Menu on 24/11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//

#include "Public/LcpContentFilter.h"

#include "IDecryptionContext.h"
#include "LcpFilterContext.h"
#include <ePub3/container.h>
#include <ePub3/filter_manager.h>
#include <ePub3/package.h>
#include <ePub3/utilities/byte_stream.h>

static std::string const LcpLicensePath = "META-INF/license.lcpl";

namespace lcp {
    bool LcpContentFilter::SniffLcpContent(ConstManifestItemPtr item)
    {
        // See for LCP detection: https://docs.google.com/document/d/1oNfXQZRSGqwpexLrhw0-0a2taEvVDXS9cPs8oBKLb0U/edit#bookmark=id.fdwd1ub4whd8
        EncryptionInfoPtr encryption = item->GetEncryptionInfo();
        return (encryption != nullptr && encryption->KeyRetrievalMethodType() == "http://readium.org/2014/01/lcp#EncryptedContentKey");
    }
    
    void *LcpContentFilter::FilterData(FilterContext *context, void *data, size_t len, size_t *outputLen)
    {
        LcpFilterContext *lcpContext = (LcpFilterContext *)context;
        unsigned char *decryptedData = new unsigned char[len];
        Status res = lcpService->DecryptData(m_license, NULL, (const unsigned char *)data, len, decryptedData, len, outputLen, lcpContext->Algorithm());
        if (!Status::IsSuccess(res)) {
            delete [] decryptedData;
            *outputLen = len;
            return data;
        }
        
        return decryptedData;
    }
    
    FilterContext *LcpContentFilter::InnerMakeFilterContext(ConstManifestItemPtr item) const
    {
        LcpFilterContext *context = new LcpFilterContext();
        
        EncryptionInfoPtr encryptionInfo = item->GetEncryptionInfo();
        if (encryptionInfo != nullptr) {
            context->SetAlgorithm(std::string(encryptionInfo->Algorithm().data()));
        }
        
        return context;
    }
    
    ContentFilterPtr LcpContentFilter::Factory(ConstPackagePtr package)
    {
        ContainerPtr container = package->GetContainer();
        if (!container->FileExistsAtPath(LcpLicensePath)) {
            return nullptr;
        }
        
        // read license.lcpl
        std::unique_ptr<ePub3::ByteStream> stream = container->ReadStreamAtPath(LcpLicensePath);
        void *buffer = nullptr;
        size_t length = stream->ReadAllBytes(&buffer);
        std::string licenseJSON((char *)buffer, length);
        free (buffer);
        
        // create content filter
        shared_ptr<LcpContentFilter> contentFilter = nullptr;
        ILicense *license;
        Status res = lcpService->OpenLicense(licenseJSON, &license);
        if (Status::IsSuccess(res)) {
            cout << "LCP: License parsed successfully <" << license->Content() << ">" << endl;
            return New(license);
        }
        
        return nullptr;
    }
    
    ILcpService *LcpContentFilter::lcpService = NULL;
    
    void LcpContentFilter::Register(ILcpService *const lcpService)
    {
        LcpContentFilter::lcpService = lcpService;
        FilterManager::Instance()->RegisterFilter("LcpFilter", MustAccessRawBytes, LcpContentFilter::Factory);
    }
}
