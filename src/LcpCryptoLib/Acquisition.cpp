#include <memory>
#include "Acquisition.h"
#include "DownloadRequest.h"
#include "Public/ILinks.h"
#include "LcpUtils.h"

namespace lcp
{
    /*static*/ const char * Acquisition::PublicationType = u8"application/epub+zip";
    /*static*/ const float Acquisition::DownloadCoefficient = 0.9;

    Acquisition::Acquisition(
        ILicense * license,
        IFileSystemProvider * fileSystemProvider,
        INetProvider * netProvider,
        const std::string & publicationPath
        )
        : m_license(license)
        , m_fileSystemProvider(fileSystemProvider)
        , m_netProvider(netProvider)
        , m_callback(nullptr)
        , m_publicationPath(publicationPath)
    {
    }

    Status Acquisition::Start(IAcquisitionCallback * callback)
    {
        try
        {
            ILinks * links = m_license->Links();
            if (!links->Has(Publication))
            {
                return Status(StCodeCover::ErrorAcquisitionNoAcquisitionLink);
            }
            Link publicationLink = links->GetLink(Publication);
            if (publicationLink.type != PublicationType)
            {
                return Status(StCodeCover::ErrorAcquisitionPublicationWrongType);
            }

            m_file.reset(m_fileSystemProvider->GetFile(m_publicationPath));
            if (m_file.get() == nullptr)
            {
                return Status(StCodeCover::ErrorAcquisitionInvalidFilePath);
            }

            m_request.reset(new DownloadRequest(publicationLink.href, m_file.get()));
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

    void Acquisition::Cancel()
    {
        m_request->SetCancelled(true);
    }

    std::string Acquisition::PublicationPath() const
    {
        return m_file->GetPath();
    }

    std::string Acquisition::SuggestedFileName() const
    {
        return m_request->SuggestedPath();
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
            m_callback->OnAcquisitionProgressed(this, progress * DownloadCoefficient);
        }
    }
    void Acquisition::OnRequestCancelled(INetRequest * request)
    {
        if (m_callback != nullptr)
        {
            m_callback->OnAcquisitionCancelled(this);
        }
    }
    void Acquisition::OnRequestEnded(INetRequest * request, Status result)
    {
        if (m_callback != nullptr)
        {
            m_callback->OnAcquisitionEnded(this, result);
        }
    }
}