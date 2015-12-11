//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#ifndef __LCP_SERVICE_H__
#define __LCP_SERVICE_H__

#include <map>
#include <memory>
#include <mutex>
#include "Public/ILcpService.h"
#include "LcpUtils.h"
#include "NonCopyable.h"

namespace lcp
{
    class RightsService;
    class JsonValueReader;
    class EncryptionProfilesManager;
    class ICryptoProvider;

    class LcpService : public ILcpService, public NonCopyable
    {
    public:
        LcpService(
            const std::string & rootCertificate,
            INetProvider * netProvider,
            IStorageProvider * storageProvider,
            IFileSystemProvider * fileSystemProvider,
            const std::string & defaultCrlUrl
            );

        // ILcpService
        virtual Status OpenLicense(const std::string & licenseJson, ILicense ** license);

        virtual Status DecryptLicense(ILicense * license, const std::string & userPassphrase);

        virtual Status DecryptData(
            ILicense * license,
            const unsigned char * data,
            const size_t dataLength,
            unsigned char * decryptedData,
            size_t inDecryptedDataLength,
            size_t * outDecryptedDataLength,
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
