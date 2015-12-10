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
                return Status(StCodeCover::ErrorAcquisitionNoAcquisitionLink);
            }
            
            links->GetLink(Publication, m_publicationLink);
            if (m_publicationLink.type != PublicationType)
            {
                return Status(StCodeCover::ErrorAcquisitionPublicationWrongType);
            }

            m_file.reset(m_fileSystemProvider->GetFile(m_publicationPath));
            if (m_file.get() == nullptr)
            {
                return Status(StCodeCover::ErrorAcquisitionInvalidFilePath);
            }

            m_request.reset(new DownloadInFileRequest(m_publicationLink.href, m_file.get()));
            m_netProvider->StartDownloadRequest(m_request.get(), this);

            return Status(StCodeCover::ErrorCommonSuccess);
        }
        catch (const StatusException & ex)
        {
            return ex.ResultStatus();
        }
        catch (const std::exception & ex)
        {
            return Status(StCodeCover::ErrorCommonFail, ex.what());
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
                return Status(StCodeCover::ErrorAcquisitionPublicationCorrupted);
            }
        }
        return Status(StCodeCover::ErrorCommonSuccess);
    }

    void Acquisition::Cancel()
    {
        m_request->SetCanceled(true);
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
                m_callback->OnAcquisitionEnded(this, Status(StCodeCover::ErrorCommonSuccess));
            }
        }
        catch (const std::exception & ex)
        {
            if (m_callback != nullptr)
            {
                m_callback->OnAcquisitionEnded(this, Status(StCodeCover::ErrorCommonFail, ex.what()));
            }
        }
    }
}