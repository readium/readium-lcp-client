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
    
    void *LcpContentFilter::FilterData(FilterContext *filterContext, void *data, size_t len, size_t *outputLen)
    {
        *outputLen = 0;
        
        LcpFilterContext *context = dynamic_cast<LcpFilterContext *>(filterContext);
        if (context == nullptr) {
            return nullptr;
        }
        
        SeekableByteStream *byteStream = context->GetSeekableByteStream();
        
        if (byteStream == nullptr) {
            // If no SeekableByteStream was passed to this filter via the
            // FilterContext object, it means that this filter should operate
            // on top of the bytes that were passed in the data parameter. What
            // that means is that, in this circumstance, this filter is one
            // filter in a chain of filters.
            uint8_t *buffer = new unsigned char[len];
            Status res = lcpService->DecryptData(m_license, NULL, (const unsigned char *)data, len, buffer, len, outputLen, context->Algorithm());
            if (!Status::IsSuccess(res)) {
                delete [] buffer;
                return nullptr;
            }
            
            return buffer;
            
        } else {
            // The SeekableByteStream is valid. That means that this filter is
            // acting alone, and it should read directly on the SeekableByteStream
            // to get the bytes to operate on. This way, this filter (and this
            // filter alone) can choose which bytes it needs to read from the
            // ePub resource.
            
            if (!byteStream->IsOpen()) {
                return nullptr;
            }
            
            ByteStream::size_type bytesToRead = 0;
            
            if (!context->GetByteRange().IsFullRange()) { // range requests only
                bytesToRead = (ByteStream::size_type)(context->GetByteRange().Length());
                byteStream->Seek(context->GetByteRange().Location(), std::ios::beg);
                
            } else { // whole file  only
                byteStream->Seek(0, std::ios::beg);
                bytesToRead = byteStream->BytesAvailable();
            }
            
            if (bytesToRead == 0) {
                return nullptr;
            }

            uint8_t *buffer = context->GetAllocateTemporaryByteBuffer(bytesToRead);
            ByteStream::size_type readBytes = byteStream->ReadBytes(buffer, bytesToRead);
            
            uint8_t *decryptedData = new unsigned char[readBytes];
            Status res = lcpService->DecryptData(m_license, context, (const unsigned char *)buffer, readBytes, decryptedData, readBytes, outputLen, context->Algorithm());
            
            
            if (!Status::IsSuccess(res)) {
                delete [] decryptedData;
                return nullptr;
            }
            
            context->AddFilteredBytesCount(readBytes);
            return decryptedData;
        }
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
