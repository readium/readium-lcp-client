#include "PublicationRequestProvider.h"
#include "Public/ILicense.h"
#include "Public/IFileSystemProvider.h"
#include "ICryptoProvider.h"
#include "DownloadRequest.h"
#include "Public/IAcquistionCallback.h"

namespace lcp
{
    /*static*/ const char * PublicationRequestProvider::PublicationType = u8"application/epub+zip";

    Status PublicationRequestProvider::CreateRequest(
        const std::string & path,
        IAcquisitionCallback * callback,
        IDownloadRequest ** request
        )
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

        m_file.reset(m_fileSystemProvider->GetFile(path));
        if (m_file.get() == nullptr)
        {
            return Status(StCodeCover::ErrorAcquisitionInvalidFilePath);
        }

        Status res = Status(StCodeCover::ErrorCommonSuccess);
        *request = new DownloadRequest(m_publicationLink.href, m_file.get());
        return res;
    }

    Status PublicationRequestProvider::CheckPublicationHash(const Link & link)
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

    void PublicationRequestProvider::OnRequestStarted(INetRequest * request)
    {
        if (m_callback != nullptr)
        {
            m_callback->OnAcquisitionStarted(this);
        }
    }

    void PublicationRequestProvider::OnRequestProgressed(INetRequest * request, float progress)
    {
        if (m_callback != nullptr)
        {
            m_callback->OnAcquisitionProgressed(this, progress);
        }
    }

    void PublicationRequestProvider::OnRequestCanceled(INetRequest * request)
    {
        if (m_callback != nullptr)
        {
            m_callback->OnAcquisitionProgressed(this, progress);
        }
    }

    void PublicationRequestProvider::OnRequestEnded(INetRequest * request, Status result)
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