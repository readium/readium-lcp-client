//
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#ifndef __LSD_PROCESSOR_H__
#define __LSD_PROCESSOR_H__

#if !DISABLE_LSD

#include <map>
#include <memory>
#include <mutex>
#include "LcpTypedefs.h"
#include "NonCopyable.h"
#include "public/ILcpService.h"

//#include <condition_variable>
//#include <public/lcp.h>
#include "public/ILinks.h"
#if ENABLE_NET_PROVIDER
#include "public/INetProvider.h"
#endif //ENABLE_NET_PROVIDER
#include "ICryptoProvider.h"
#include "EncryptionProfilesManager.h"

#if USE_MEMORY_NOT_FILE
    #include "SimpleMemoryWritableStream.h"
#else
    #include "public/IFileSystemProvider.h"
#endif //USE_MEMORY_NOT_FILE


namespace lcp
{
    class JsonValueReader;

    // https://github.com/drminside/lsd_client/blob/master/json_schema_lsd.json
    // https://github.com/drminside/lsd_client/blob/master/lsd_client.py#L453
    // https://docs.google.com/document/d/1ErBf0Gl32jNH-QVKWpGPfZDMWMeQP7dH9YY5g7agguQ
    // https://docs.google.com/document/d/1VKkAG9aKbKLQYnSjSa5-_cOgnVFsgU9a7bHO0-IkRgk
    // https://docs.google.com/document/d/1oNfXQZRSGqwpexLrhw0-0a2taEvVDXS9cPs8oBKLb0U
    class LsdProcessor : public NonCopyable
#if ENABLE_NET_PROVIDER
            , public INetProviderCallback
#endif //ENABLE_NET_PROVIDER
    {
#if ENABLE_NET_PROVIDER
    public:
        // INetProviderCallback
        virtual void OnRequestStarted(INetRequest * request);
        virtual void OnRequestProgressed(INetRequest * request, float progress);
        virtual void OnRequestCanceled(INetRequest * request);
        virtual void OnRequestEnded(INetRequest * request, Status result);
#endif //ENABLE_NET_PROVIDER

    public:
        static std::string StatusType;
    public:
        LsdProcessor(ILcpService * lcpService, ILicense* license, std::string publicationPath);
    private:
        ILcpService * m_lcpService;
        ILicense * m_license;
        std::string m_publicationPath;

    private:
        std::unique_ptr<JsonValueReader> m_jsonReader;
        std::unique_ptr<EncryptionProfilesManager> m_encryptionProfilesManager;
        std::unique_ptr<ICryptoProvider> m_cryptoProvider;

    private:
        std::string ResolveTemplatedURL(ILicense* license, const std::string & url);
        Status CheckStatusDocumentHash(IReadableStream* stream);

        Status CheckLicenseStatusDocument(ILicense* license);


        mutable std::mutex m_lsdSync;
        lcp::Link m_lsdLink;
#if ENABLE_NET_PROVIDER
        std::unique_ptr<IDownloadRequest> m_lsdRequest;
#endif //ENABLE_NET_PROVIDER
        std::string m_lsdNewLcpLicenseString;
#if ENABLE_NET_PROVIDER
        bool m_lsdRequestRunning;
        lcp::Status m_lsdRequestStatus;
#endif //ENABLE_NET_PROVIDER
        //std::condition_variable m_lsdCondition;
        //std::string m_lsdPath;
        //std::unique_ptr<IFile> m_lsdFile;
#if USE_MEMORY_NOT_FILE
        std::unique_ptr<SimpleMemoryWritableStream> m_lsdStream;
#else
        std::unique_ptr<IFile> m_lsdFile;
#endif //USE_MEMORY_NOT_FILE
    };



#if USE_MEMORY_NOT_FILE
    class BufferRedableStreamAdapter : public IReadableStream
    {
    public:
        BufferRedableStreamAdapter(const std::vector<unsigned char> &buffer) : m_buffer(buffer), m_pos(0) {}

        virtual void Read(unsigned char * pBuffer, int64_t sizeToRead)
        {
            int64_t available = this->Size() - m_pos;
            if (sizeToRead <= 0) {
                return;
            }
            if (sizeToRead > available) {
                sizeToRead = available;
            }
            //std::memcpy(pBuffer, sizeToRead);
            for (int i = 0; i < sizeToRead; i++) {
                char c = m_buffer[m_pos+i];
                *(pBuffer+i) = c;
            }
        }

        virtual void SetReadPosition(int64_t pos)
        {
            m_pos = pos;
            if (m_pos < 0) {
                m_pos = 0;
            } else if (m_pos >= this->Size()) {
                m_pos = (this->Size()-1);
            }
        }

        virtual int64_t ReadPosition() const
        {
            return m_pos;
        }

        virtual int64_t Size()
        {
            return m_buffer.size();
        }

    protected:
        std::vector<unsigned char> m_buffer;
        int64_t m_pos;
    };
#endif //USE_MEMORY_NOT_FILE
}

#endif //!DISABLE_LSD

#endif //__LSD_PROCESSOR_H__
