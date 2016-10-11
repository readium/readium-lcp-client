//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#ifndef __LCP_SERVICE_H__
#define __LCP_SERVICE_H__

#include <condition_variable>
#include <map>
#include <memory>
#include <mutex>
#include <future>
#include "LcpTypedefs.h"
#include "NonCopyable.h"
#include "public/ILcpService.h"

#if !DISABLE_LSD
//#include <public/lcp.h>
#include "public/ILinks.h"
#include "public/INetProvider.h"
#if USE_MEMORY_NOT_FILE
    #include "SimpleMemoryWritableStream.h"
#else
    #include <public/IFileSystemProvider.h>
#endif //USE_MEMORY_NOT_FILE
#endif //!DISABLE_LSD

namespace lcp
{
    class RightsService;
    class JsonValueReader;
    class EncryptionProfilesManager;
    class ICryptoProvider;

    class LcpService : public ILcpService, public NonCopyable
#if !DISABLE_LSD
    ,
    public INetProviderCallback
#endif //!DISABLE_LSD
    {
#if !DISABLE_LSD
    public:
        // INetProviderCallback
        virtual void OnRequestStarted(INetRequest * request);
        virtual void OnRequestProgressed(INetRequest * request, float progress);
        virtual void OnRequestCanceled(INetRequest * request);
        virtual void OnRequestEnded(INetRequest * request, Status result);
    private:
        Status ProcessLicenseStatusDocument(ILicense** license, std::promise<ILicense*> & licensePromise);
        std::string ResolveTemplatedURL(const std::string & url);
        Status CheckStatusDocumentHash(IReadableStream* stream);

        static std::string StatusType;
        bool m_lsdRequestRunning;
        lcp::Status m_lsdRequestStatus;
        mutable std::mutex m_lsdSync;
        std::condition_variable m_lsdCondition;
        lcp::Link m_lsdLink;
        //std::string m_lsdPath;
        //std::unique_ptr<IFile> m_lsdFile;
        std::unique_ptr<IDownloadRequest> m_lsdRequest;
        ePub3::string m_publicationPath;
        std::string m_lsdNewLcpLicenseString;
#if USE_MEMORY_NOT_FILE
        std::unique_ptr<SimpleMemoryWritableStream> m_lsdStream;
#else
        std::unique_ptr<IFile> m_lsdFile;
#endif //USE_MEMORY_NOT_FILE
        ILicense * m_lsdOriginalLicense;
        std::promise<ILicense*> * m_lsdLicensePromise;
#endif //!DISABLE_LSD

    private:
        Status CheckDecrypted(ILicense* license);

    public:
        LcpService(
            const std::string & rootCertificate,
            INetProvider * netProvider,
            IStorageProvider * storageProvider,
            IFileSystemProvider * fileSystemProvider,
            const std::string & defaultCrlUrl
            );

        // ILcpService
        virtual Status OpenLicense(
#if !DISABLE_LSD
                const ePub3::string & publicationPath,
#endif //!DISABLE_LSD
                const std::string & licenseJson, std::promise<ILicense*> & licensePromise);

        virtual Status DecryptLicense(ILicense * license, const std::string & userPassphrase);

        virtual Status DecryptData(
            ILicense * license,
            const unsigned char * data,
            const size_t dataLength,
            unsigned char * decryptedData,
            size_t * decryptedDataLength,
            const std::string & algorithm
            );

        virtual Status CreateEncryptedDataStream(
            ILicense * license,
            IReadableStream * stream,
            const std::string & algorithm,
            IEncryptedStream ** encStream
            );

        virtual Status AddUserKey(const std::string & userKey);
        virtual Status AddUserKey(
            const std::string & userKey,
            const std::string & userId,
            const std::string & providerId
            );
        virtual Status AddUserKey(
            const std::string & userKey,
            const std::string & userId,
            const std::string & providerId,
            const std::string & licenseId
            );

        virtual Status CreatePublicationAcquisition(
            const std::string & publicationPath,
            ILicense * license,
            IAcquisition ** acquisition
            );

        virtual IRightsService * GetRightsService() const;

        virtual std::string RootCertificate() const;
        virtual INetProvider * NetProvider() const;
        virtual IStorageProvider * StorageProvider() const;
        virtual IFileSystemProvider * FileSystemProvider() const;

    private:
        bool FindLicense(const std::string & canonicalJson, ILicense ** license);
        
        Status DecryptLicenseOnOpening(ILicense * license);
        Status DecryptLicenseByUserKey(ILicense * license, const KeyType & userKey);
        Status DecryptLicenseByHexUserKey(ILicense * license, const std::string & hexUserKey);
        Status DecryptLicenseByStorage(ILicense * license);
        Status AddDecryptedUserKey(ILicense * license, const KeyType & userKey);

        std::string CalculateCanonicalForm(const std::string & licenseJson);
        std::string BuildStorageProviderKey(ILicense * license);
        std::string BuildStorageProviderKey(
            const std::string & providerId,
            const std::string & userId,
            const std::string & licenseId
            );

    private:
        std::string m_rootCertificate;
        INetProvider * m_netProvider;
        IStorageProvider * m_storageProvider;
        IFileSystemProvider * m_fileSystemProvider;

        std::unique_ptr<RightsService> m_rightsService;
        std::unique_ptr<JsonValueReader> m_jsonReader;
        std::unique_ptr<EncryptionProfilesManager> m_encryptionProfilesManager;
        std::unique_ptr<ICryptoProvider> m_cryptoProvider;
        std::map<std::string, std::unique_ptr<ILicense> > m_licenses;
        std::mutex m_licensesSync;

    private:
        static std::string UnknownProvider;
        static std::string UnknownUserId;
    };
}
#endif //__LCP_SERVICE_H__
