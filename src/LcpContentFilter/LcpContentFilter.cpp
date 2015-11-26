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
        Status res = lcpService->DecryptData(m_license.get(), NULL, (const unsigned char *)data, len, decryptedData, len, outputLen, lcpContext->Algorithm());
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
        unique_ptr<ArchiveReader> licenseReader = package->GetContainer()->GetArchive()->ReaderAtPath("META-INF/license.lcpl");
        if (!licenseReader) {
            std::cout << "LCP: No license.lcpl found in the package" << std::endl;
            return nullptr;
        }
        
        std::shared_ptr<LcpContentFilter> contentFilter = nullptr;
        
        size_t licenseSize = licenseReader->total_size();
        unsigned char *licenseJSON = new unsigned char[licenseSize];
        
        if (licenseReader->read(licenseJSON, licenseSize) == -1) {
            std::cout << "LCP: Failed to read license.lcpl" << std::endl;
            
        } else {
            ILicense * rawLicPtr = nullptr;
            Status res = lcpService->OpenLicense(std::string(reinterpret_cast<char*>(licenseJSON)), &rawLicPtr);
            if (Status::IsSuccess(res) || res.ResultCode == StCodeCover::StatusCode::ErrorOpeningLicenseStillEncrypted) {
                std::cout << "LCP: License parsed successfully <" << rawLicPtr->Content() << ">" << std::endl;
                
                if (!rawLicPtr->Decrypted()) {
                    res = lcpService->DecryptLicense(rawLicPtr, "White whales are huge!");
                    if (Status::IsSuccess(res)) {
                        std::cout << "LCP: License decrypted successfully!" << std::endl;
                    } else {
                        std::cout << "LCP: Failed to decrypt license: " << res.ResultCode << " <" << res.Extension << ">" << std::endl;
                    }
                    
                } else {
                    std::cout << "LCP: License decrypted successfully by user key from the storage!" << std::endl;
                }
                
                contentFilter = New(std::shared_ptr<ILicense>(rawLicPtr));
            }
        }
        
        free(licenseJSON);
        return contentFilter;
    }
    
    std::shared_ptr<ILcpService> LcpContentFilter::lcpService = nullptr;
    
    void LcpContentFilter::Register(std::shared_ptr<ILcpService> lcpService)
    {
        LcpContentFilter::lcpService = lcpService;
        FilterManager::Instance()->RegisterFilter("LcpFilter", MustAccessRawBytes, LcpContentFilter::Factory);
    }
}
