//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#ifndef __LCP_SERVICE_H__
#define __LCP_SERVICE_H__

#include <map>
#include <memory>
#include <mutex>
#include "LcpTypedefs.h"
#include "NonCopyable.h"
#include "public/ILcpService.h"

namespace lcp
{
    class RightsService;
    class JsonValueReader;
    class EncryptionProfilesManager;
    class ICryptoProvider;

    class LcpService : public ILcpService, public NonCopyable
    {
    private:
        std::string m_publicationPath;
        Status CheckDecrypted(ILicense* license);
#if !DISABLE_LSD
        Status CheckLicenseStatusDocument(ILicense* license);
#endif //!DISABLE_LSD

    public:
        LcpService(
            const std::string & rootCertificate,
#if ENABLE_NET_PROVIDER
            INetProvider * netProvider,
#endif //ENABLE_NET_PROVIDER
            IStorageProvider * storageProvider,
            IFileSystemProvider * fileSystemProvider
#if ENABLE_NET_PROVIDER
                ,
            const std::string & defaultCrlUrl
#endif //ENABLE_NET_PROVIDER
            );

        // ILcpService
        virtual Status OpenLicense(
                const std::string & publicationPath,
                const std::string & licenseJson,
                ILicense** licensePTR);

        virtual Status InjectLicense(
                const std::string & publicationPath,
                const std::string & licenseJson);

        virtual Status InjectLicense(
                const std::string & publicationPath,
                ILicense * license);

        virtual int TimeStampCompare(const std::string & t1, const std::string & t2);

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
#if ENABLE_NET_PROVIDER
        virtual Status CreatePublicationAcquisition(
                const std::string & publicationPath,
                ILicense * license,
                IAcquisition ** acquisition
        );
#endif //ENABLE_NET_PROVIDER

//#if !DISABLE_LSD
//        virtual Status CreatePublicationStatusDocumentProcessing(
//                const std::string & publicationPath,
//                ILicense * license,
//                IStatusDocumentProcessing ** statusDocumentProcessing
//        );
//#endif //!DISABLE_LSD

        virtual IRightsService * GetRightsService() const;

        virtual std::string RootCertificate() const;
#if ENABLE_NET_PROVIDER
        virtual INetProvider * NetProvider() const;
#endif //ENABLE_NET_PROVIDER
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
#if ENABLE_NET_PROVIDER
        INetProvider * m_netProvider;
#endif //ENABLE_NET_PROVIDER
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
