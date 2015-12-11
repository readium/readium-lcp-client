//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#include <memory>
#include <sstream>
#include "ziplib/Source/ZipLib/ZipFile.h"
#include "Acquisition.h"
#include "DownloadInFileRequest.h"
#include "LcpUtils.h"
#include "Public/ILicense.h"
#include "ICryptoProvider.h"

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
            m_callback = callback;

            ILinks * links = m_license->Links();
            if (!links->Has(Publication))
            {
                return Status(StatusCode::ErrorAcquisitionNoAcquisitionLink);
            }
            
            links->GetLink(Publication, m_publicationLink);
            if (m_publicationLink.type != PublicationType)
            {
                return Status(StatusCode::ErrorAcquisitionPublicationWrongType);
            }

            m_file.reset(m_fileSystemProvider->GetFile(m_publicationPath));
            if (m_file.get() == nullptr)
            {
                return Status(StatusCode::ErrorAcquisitionInvalidFilePath);
            }

            m_request.reset(new DownloadInFileRequest(m_publicationLink.href, m_file.get()));
            m_netProvider->StartDownloadRequest(m_request.get(), this);

            return Status(StatusCode::ErrorCommonSuccess);
        }
        catch (const StatusException & ex)
        {
            return ex.ResultStatus();
        }
        catch (const std::exception & ex)
        {
            return Status(StatusCode::ErrorCommonFail, ex.what());
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
                return Status(StatusCode::ErrorAcquisitionPublicationCorrupted);
            }
        }
        return Status(StatusCode::ErrorCommonSuccess);
    }

    Status Acquisition::Cancel()
    {
        try
        {
            m_request->SetCanceled(true);
            return Status(StatusCode::ErrorCommonSuccess);
        }
        catch (const std::exception & ex)
        {
            return Status(StatusCode::ErrorCommonFail, ex.what());
        }
    }

    std::string Acquisition::PublicationPath() const
    {
        return m_publicationPath;
    }

    std::string Acquisition::SuggestedFileName() const
    {
        return m_request->SuggestedFileName();
    }

    void Acquisition::OnRequestStarted(INetRequest * request)
    {
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
        if (m_callback != nullptr)
        {
            m_callback->OnAcquisitionCanceled(this);
        }
    }

    void Acquisition::OnRequestEnded(INetRequest * request, Status result)
    {
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
            ZipFile::AddFile(m_publicationPath, licenseStream, "META-INF/license.lcpl");

            if (m_callback != nullptr)
            {
                m_callback->OnAcquisitionEnded(this, Status(StatusCode::ErrorCommonSuccess));
            }
        }
        catch (const std::exception & ex)
        {
            if (m_callback != nullptr)
            {
                m_callback->OnAcquisitionEnded(this, Status(StatusCode::ErrorCommonFail, ex.what()));
            }
        }
    }
}