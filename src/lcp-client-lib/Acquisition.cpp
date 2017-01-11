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


#if ENABLE_NET_PROVIDER_ACQUISITION

#include "public/ILicense.h"
#include "Acquisition.h"
#include "DownloadInFileRequest.h"
#include "IncludeMacros.h"
#include "ICryptoProvider.h"
#include "LcpUtils.h"
#include <memory>
#include <sstream>

ZIPLIB_INCLUDE_START
#include "ziplib/Source/ZipLib/ZipFile.h"
ZIPLIB_INCLUDE_END

static std::string const LcpLicensePath = "META-INF/license.lcpl";

namespace lcp
{
    /*static*/ const char * Acquisition::PublicationType = u8"application/epub+zip";

    Acquisition::Acquisition(
        ILicense * license,
        IFileSystemProvider * fileSystemProvider,
        INetProvider * netProvider,
        ICryptoProvider * cryptoProvider,
        const std::string & publicationPath
        )
        : m_license(license)
        , m_fileSystemProvider(fileSystemProvider)
        , m_netProvider(netProvider)
        , m_cryptoProvider(cryptoProvider)
        , m_callback(nullptr)
        , m_publicationPath(publicationPath)
    {
    }

    Status Acquisition::Start(IAcquisitionCallback * callback)
    {
        try
        {
            std::unique_lock<std::mutex> locker(m_sync);

            m_callback = callback;

            ILinks * links = m_license->Links();
            if (!links->Has(Publication))
            {
                return Status(StatusCode::ErrorAcquisitionNoAcquisitionLink, "ErrorAcquisitionNoAcquisitionLink");
            }
            
            links->GetLink(Publication, m_publicationLink);
            if (m_publicationLink.type != PublicationType)
            {
                return Status(StatusCode::ErrorAcquisitionPublicationWrongType, "ErrorAcquisitionPublicationWrongType");
            }

            m_file.reset(m_fileSystemProvider->GetFile(m_publicationPath));
            if (m_file.get() == nullptr)
            {
                return Status(StatusCode::ErrorAcquisitionInvalidFilePath, "ErrorAcquisitionInvalidFilePath");
            }
            m_request.reset(new DownloadInFileRequest(m_publicationLink.href, m_file.get()));

            locker.unlock();
            
            m_netProvider->StartDownloadRequest(m_request.get(), this);

            return Status(StatusCode::ErrorCommonSuccess);
        }
        catch (const StatusException & ex)
        {
            return ex.ResultStatus();
        }
    }

    Status Acquisition::CheckPublicationHash(const Link & link)
    {
        if (!link.hash.empty())
        {
            std::vector<unsigned char> rawHash;
            Status res = m_cryptoProvider->CalculateFileHash(m_file.get(), rawHash);
            if (!Status::IsSuccess(res))
                return res;

            std::string hexHash;
            res = m_cryptoProvider->ConvertRawToHex(rawHash, hexHash);
            if (!Status::IsSuccess(res))
                return res;

            if (hexHash != link.hash)
            {
                return Status(StatusCode::ErrorAcquisitionPublicationCorrupted, "ErrorAcquisitionPublicationCorrupted");
            }
        }
        return Status(StatusCode::ErrorCommonSuccess);
    }

    Status Acquisition::Cancel()
    {
        std::unique_lock<std::mutex> locker(m_sync);
        m_request->SetCanceled(true);
        m_netProvider->CancelDownloadRequest(m_request.get());
        return Status(StatusCode::ErrorCommonSuccess);
    }

    std::string Acquisition::PublicationPath() const
    {
        return m_publicationPath;
    }

    std::string Acquisition::SuggestedFileName() const
    {
        std::unique_lock<std::mutex> locker(m_sync);
        return m_request->SuggestedFileName();
    }

    void Acquisition::OnRequestStarted(INetRequest * request)
    {
        std::unique_lock<std::mutex> locker(m_sync);
        if (m_callback != nullptr)
        {
            m_callback->OnAcquisitionStarted(this);
        }
    }

    void Acquisition::OnRequestProgressed(INetRequest * request, float progress)
    {
        if (m_callback != nullptr)
        {
            m_callback->OnAcquisitionProgressed(this, progress);
        }
    }

    void Acquisition::OnRequestCanceled(INetRequest * request)
    {
        std::unique_lock<std::mutex> locker(m_sync);
        if (m_callback != nullptr)
        {
            m_callback->OnAcquisitionCanceled(this);
        }
    }

    void Acquisition::OnRequestEnded(INetRequest * request, Status result)
    {
        std::unique_lock<std::mutex> locker(m_sync);
        try
        {
            if (!Status::IsSuccess(result))
            {
                if (m_callback != nullptr)
                {
                    m_callback->OnAcquisitionEnded(this, result);
                }
                return;
            }

            Status hashCheckResult = this->CheckPublicationHash(m_publicationLink);
            if (!Status::IsSuccess(hashCheckResult))
            {
                if (m_callback != nullptr)
                {
                    m_callback->OnAcquisitionEnded(this, hashCheckResult);
                }
                return;
            }

            // Release the epub file to perform zip operations
            m_file.reset();
            std::stringstream licenseStream(m_license->OriginalContent());
            ZipFile::AddFile(m_publicationPath, licenseStream, LcpLicensePath);

            if (m_callback != nullptr)
            {
                m_callback->OnAcquisitionEnded(this, Status(StatusCode::ErrorCommonSuccess));
            }
        }
        catch (const std::exception & ex)
        {
            if (m_callback != nullptr)
            {
                m_callback->OnAcquisitionEnded(this, Status(StatusCode::ErrorNetworkingRequestFailed, "ErrorNetworkingRequestFailed" + std::string(ex.what())));
            }
        }
    }
}

#endif //ENABLE_NET_PROVIDER_ACQUISITION