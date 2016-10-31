//
//  Created by Mickaël Menu on 24/11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#if FEATURES_READIUM

#include "public/LcpContentFilter.h"

#include "IDecryptionContext.h"
#include "LcpFilterContext.h"
#include "LcpSeekableByteStreamAdapter.h"
#include "StreamInterfaces.h"

READIUM_INCLUDE_START
#include <ePub3/container.h>
#include <ePub3/filter_manager.h>
#include <ePub3/media-overlays_smil_utils.h>
//#include <cmath>
#include <ePub3/package.h>
#include <ePub3/utilities/byte_stream.h>
#include <zlib.h>
#include <ePub3/utilities/byte_buffer.h>

READIUM_INCLUDE_END

#if DEBUG
#define LOG(msg) std::cout << "LCP: " << msg << std::endl
#else
#define LOG(msg)
#endif

//static std::string const LcpLicensePath = "META-INF/license.lcpl";

namespace lcp {
    bool LcpContentFilter::SniffLcpContent(ConstManifestItemPtr item)
    {
        // See for LCP detection: https://docs.google.com/document/d/1oNfXQZRSGqwpexLrhw0-0a2taEvVDXS9cPs8oBKLb0U/edit#bookmark=id.fdwd1ub4whd8
        EncryptionInfoPtr encryption = item->GetEncryptionInfo();
        bool encrypted = (encryption != nullptr && encryption->KeyRetrievalMethodType() == "http://readium.org/2014/01/lcp#EncryptedContentKey");
        
//        LOG("Resource <" << item->Href() << "> " << (encrypted ? "is" : "is not") << " encrypted");
        
        return encrypted;
    }

    uint8_t* checkAndProcessDeflateBuffer(uint8_t* buffer, size_t* outputLen, LcpFilterContext *context) {

        if (context->CompressionMethod() == "8") {

            z_stream zstr = {0};

            zstr.zalloc = (alloc_func)Z_NULL;
            zstr.zfree = (free_func)Z_NULL;

            zstr.opaque = (voidpf)NULL;

            zstr.total_in = 0;
            zstr.total_out = 0;

            zstr.next_in = (Bytef *)buffer;
            zstr.avail_in = *outputLen;

            int64_t inflatedBufferLength = 16*1024; // TODO: should be 128KB ? (inflate seems to work fine in 16KB chunks)

            //uint8_t* inflatedBuffer = new uint8_t[inflatedBufferLength];
            uint8_t inflatedBuffer[inflatedBufferLength];

            zstr.next_out = (Bytef *)&inflatedBuffer;
            zstr.avail_out = inflatedBufferLength; //sizeof(inflatedBuffer)

            ePub3::ByteBuffer byteBuffer;
            //byteBuffer.EnsureCapacity(bytesToRead);

            int rez = inflateInit2(&zstr, -MAX_WBITS); //(MAX_WBITS + 32)); //15 window bits, and the +32 tells zlib to to detect if using gzip or zlib
            if (rez == Z_OK) {

                while (true) {
                    zstr.next_out = (Bytef *)&inflatedBuffer;
                    zstr.avail_out = inflatedBufferLength; //sizeof(inflatedBuffer)

                    int ress = inflate(&zstr, Z_FINISH);

                    if (ress != Z_STREAM_END && ress != Z_OK && ress != Z_BUF_ERROR) {
                        inflateEnd(&zstr);
                        //delete[] inflatedBuffer;
                        return nullptr;
                    }

                    int toAdd = inflatedBufferLength - zstr.avail_out;
                    if (toAdd > 0) {
                        byteBuffer.AddBytes(inflatedBuffer, toAdd);
                    }

                    if (ress == Z_STREAM_END) {
                        break;
                    }
                }
            }
            else {
                inflateEnd(&zstr);
                //delete[] inflatedBuffer;
                return nullptr;
            }

            inflateEnd(&zstr);
            //delete[] inflatedBuffer;

            *outputLen = byteBuffer.GetBufferSize();

            const ePub3::string sizeStr = context->OriginalLength();
            uint32_t sizeDeclared = (uint32_t) floor(SmilClockValuesParser::ToSeconds(sizeStr));
            size_t sizeActual = *outputLen;
            if (sizeDeclared != sizeActual) {
                std::cout << "LCP compress-before-encrypt wrong length: " << sizeStr << " => " << sizeDeclared << " vs. " << sizeActual << std::endl;
            }

            //not good, as the returned buffer pointer will be freed when byteBuffer is destroyed (stack memory)
            //buffer = byteBuffer.GetBytes();

            // note that when existing temp buffer (size == bytesToRead) is too small, new one is allocated.
            buffer = context->GetAllocateTemporaryByteBuffer(*outputLen);
            std::memcpy(buffer, byteBuffer.GetBytes(), *outputLen);
        }

        return buffer;
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

            Status res = LcpContentFilter::lcpService->DecryptData(m_license, (const unsigned char *)data, len, buffer, &bufferLen, context->Algorithm());
            if (!Status::IsSuccess(res)) {
                delete [] buffer;
                return nullptr;
            }
            
            *outputLen = bufferLen;

            buffer = checkAndProcessDeflateBuffer(buffer, outputLen, context);

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

            IReadableStream *readableStream = new SeekableByteStreamAdapter(byteStream);

            IEncryptedStream *encryptedStream;
            Status status = LcpContentFilter::lcpService->CreateEncryptedDataStream(m_license, readableStream, context->Algorithm(), &encryptedStream);
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

            buffer = checkAndProcessDeflateBuffer(buffer, outputLen, context);

            return buffer;
        }
    }
    
    ByteStream::size_type LcpContentFilter::BytesAvailable(FilterContext *filterContext, SeekableByteStream *byteStream) const
    {
        LcpFilterContext *context = dynamic_cast<LcpFilterContext *>(filterContext);
        if (context != nullptr) {
            SeekableByteStreamAdapter *readableStream = new SeekableByteStreamAdapter(byteStream);
            IEncryptedStream *encryptedStream;
            Status status = LcpContentFilter::lcpService->CreateEncryptedDataStream(m_license, readableStream, context->Algorithm(), &encryptedStream);
            
            if (Status::IsSuccess(status)) {

                if (context->CompressionMethod() == "8") {

                    const ePub3::string sizeStr = context->OriginalLength();
                    uint32_t sizeOriginal = (uint32_t) floor(SmilClockValuesParser::ToSeconds(sizeStr));
                    size_t sizeBeforeDecompressInflate = encryptedStream->DecryptedSize(); //  should be smaller than sizeDeclared
                    if (sizeBeforeDecompressInflate >= sizeOriginal) {
                        std::cout << "LCP compress-before-encrypt incorrect length? " << sizeStr << " => " << sizeOriginal << " <= " << sizeBeforeDecompressInflate << std::endl;
                    }
                    return sizeOriginal;

                } else {
                    return encryptedStream->DecryptedSize();
                }
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
            filterContext->SetCompressionInfo(encryptionInfo->CompressionMethod(), encryptionInfo->UnCompressedSize()); //OriginalLength
        }
        
        return filterContext;
    }
    
    ContentFilterPtr LcpContentFilter::Factory(ConstPackagePtr package)
    {
        if (LcpContentFilter::lcpLicense != NULL) {
            return std::make_shared<LcpContentFilter>(LcpContentFilter::lcpLicense); //New(LcpContentFilter::lcpLicense);
        }
        return nullptr;
    }

    // static
    ILicense *LcpContentFilter::lcpLicense = NULL;

    // static
    ILcpService *LcpContentFilter::lcpService = NULL;
    
    void LcpContentFilter::Register(ILcpService *const service, ILicense *const license)
    {
        LcpContentFilter::lcpService = service;
        LcpContentFilter::lcpLicense = license;
        FilterManager::Instance()->RegisterFilter("LcpFilter", MustAccessRawBytes, LcpContentFilter::Factory);
    }
}

#endif // FEATURES_READIUM

