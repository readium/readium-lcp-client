// Copyright (c) 2016 Mantano
// Licensed to the Readium Foundation under one or more contributor license agreements.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation and/or
//    other materials provided with the distribution.
// 3. Neither the name of the organization nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.



#if FEATURES_READIUM

#include "public/LcpContentFilter.h"

#include "IDecryptionContext.h"
#include "LcpFilterContext.h"
#include "LcpSeekableByteStreamAdapter.h"
#include "StreamInterfaces.h"

READIUM_INCLUDE_START
#include <ePub3/container.h>
#include <ePub3/filter_manager.h>
#include <ePub3/package.h>
#include <ePub3/utilities/byte_stream.h>
READIUM_INCLUDE_END

#if DEBUG
#define LOG(msg) std::cout << "LCP: " << msg << std::endl
#else
#define LOG(msg)
#endif

static std::string const LcpLicensePath = "META-INF/license.lcpl";

namespace lcp {
    bool LcpContentFilter::SniffLcpContent(ConstManifestItemPtr item)
    {
        // See for LCP detection: https://docs.google.com/document/d/1oNfXQZRSGqwpexLrhw0-0a2taEvVDXS9cPs8oBKLb0U/edit#bookmark=id.fdwd1ub4whd8
        EncryptionInfoPtr encryption = item->GetEncryptionInfo();
        bool encrypted = (encryption != nullptr && encryption->KeyRetrievalMethodType() == "http://readium.org/2014/01/lcp#EncryptedContentKey");
        
//        LOG("Resource <" << item->Href() << "> " << (encrypted ? "is" : "is not") << " encrypted");
        
        return encrypted;
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
            size_t bufferLen = len;
            uint8_t *buffer = new unsigned char[bufferLen];
            Status res = lcpService->DecryptData(m_license, (const unsigned char *)data, len, buffer, &bufferLen, context->Algorithm());
            if (!Status::IsSuccess(res)) {
                delete [] buffer;
                return nullptr;
            }
            
            *outputLen = bufferLen;
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
            
            SeekableByteStreamAdapter *readableStream = new SeekableByteStreamAdapter(byteStream);
            
            IEncryptedStream *encryptedStream;
            Status status = lcpService->CreateEncryptedDataStream(m_license, readableStream, context->Algorithm(), &encryptedStream);
            if (!Status::IsSuccess(status)) {
                LOG("Failed to create readable stream");
                return nullptr;
            }
            
            ByteStream::size_type bytesToRead = 0;
            if (!context->GetByteRange().IsFullRange()) { // range requests only
                encryptedStream->SetReadPosition(context->GetByteRange().Location());
                bytesToRead = (ByteStream::size_type)(context->GetByteRange().Length());
                
            } else { // whole file  only
                encryptedStream->SetReadPosition(0);
                bytesToRead = encryptedStream->DecryptedSize();
            }
            
            if (bytesToRead == 0) {
                return nullptr;
            }
            
            uint8_t *buffer = context->GetAllocateTemporaryByteBuffer(bytesToRead);
            encryptedStream->Read(buffer, bytesToRead);
            *outputLen = bytesToRead;
            
            return buffer;
        }
    }
    
    ByteStream::size_type LcpContentFilter::BytesAvailable(FilterContext *filterContext, SeekableByteStream *byteStream) const
    {
        LcpFilterContext *context = dynamic_cast<LcpFilterContext *>(filterContext);
        if (context != nullptr) {
            SeekableByteStreamAdapter *readableStream = new SeekableByteStreamAdapter(byteStream);
            IEncryptedStream *encryptedStream;
            Status status = lcpService->CreateEncryptedDataStream(m_license, readableStream, context->Algorithm(), &encryptedStream);
            
            if (Status::IsSuccess(status)) {
                return encryptedStream->DecryptedSize();
            } else {
                LOG("Failed to create readable stream");
            }
        }
        
        return byteStream->BytesAvailable();
    }
    
    FilterContext *LcpContentFilter::InnerMakeFilterContext(ConstManifestItemPtr item) const
    {
        LcpFilterContext *filterContext = new LcpFilterContext();
        
        EncryptionInfoPtr encryptionInfo = item->GetEncryptionInfo();
        if (encryptionInfo != nullptr) {
            filterContext->SetAlgorithm(std::string(encryptionInfo->Algorithm().data()));
        }
        
        return filterContext;
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
            return New(license);
        } else {
            LOG("Failed to parse license <" << res.Code << ": " << res.Extension << ">");
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

#endif // FEATURES_READIUM

