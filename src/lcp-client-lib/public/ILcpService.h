//
//  Created by Mickaël Menu, Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#ifndef __I_LCP_SERVICE_H__
#define __I_LCP_SERVICE_H__

#include <string>
#include "LcpStatus.h"
#include "IStatusDocumentProcessing.h"

#if !DISABLE_LSD
#include <ePub3/utilities/utfstring.h>
#endif //!DISABLE_LSD

namespace lcp
{
    class ILicense;
    class INetProvider;
    class IStorageProvider;
    class IFileSystemProvider;
    class IAcquisition;
    class IAcquisitionCallback;
    class IRightsService;
    class IReadableStream;
    class IEncryptedStream;

    class IClientProvider
    {
    public:
        //
        // Root certificate used to validate the Content Provider certificates
        // Usually provided by the Readium foundation to LCP certified
        // licensees.
        //
        virtual std::string RootCertificate() const = 0;

        //
        // Shared implementation of the net provider, used for any network
        // call from the library.
        //
        virtual INetProvider * NetProvider() const = 0;

        //
        // Shared implementation of the secure storage provider, used to store
        // and retrieve safely any string value by the library.
        //
        virtual IStorageProvider * StorageProvider() const = 0;

        //
        // Shared implementation of the file system provider, used for
        // any calls to file system.
        // If it's not set by the client, default implementation is used
        //
        virtual IFileSystemProvider * FileSystemProvider() const = 0;

        virtual ~IClientProvider() {}
    };

    class ILcpService : public IClientProvider
    {
    public:
        //
        // Parses the given JSON License Document and returns a matching
        // License instance. The service is the owner of all created ILicense
        // instances, and subsequent calls with the same canonical JSON will
        // always return the same instance.
        // The License will be automatically decrypted if a valid User Key can
        // be found in the storage provider.
        //
        virtual Status OpenLicense(
                const ePub3::string & publicationPath,
                const std::string & licenseJson,
                ILicense** license) = 0;

        //
        // Decrypts the License Document using the given User Passphrase.
        // If the resulting User Key is valid, it will be permanently 
        // stored inside the storage provider for future retrieval.
        //
        virtual Status DecryptLicense(ILicense * license, const std::string & userPassphrase) = 0;

        //
        // Decrypts the input data using the given License.
        // The decrypted data will be stored into the decryptedData buffer.
        // You are responsible for allocating and freeing memory of the
        // decryptedData buffer. You can use an initial size equal to the size
        // of the encrypted data, as it will be smaller once decrypted.
        // You muse set the initial size of the buffer in decryptedDataLength.
        // This value will be overwritten once the data is decrypted to contain
        // the real length of the decrypted data.
        // Algorithm parameter must be the same as in the Encryption Profile.
        //
        virtual Status DecryptData(
            ILicense * license,
            const unsigned char * data,
            const size_t dataLength,
            unsigned char * decryptedData,
            size_t * decryptedDataLength,
            const std::string & algorithm
            ) = 0;

        //
        // Creates a new stream from a given encrypted stream that will
        // decrypt dynamically the content when accessed. You must not read
        // more than IEncryptedStream::DecryptedSize() bytes from the created
        // stream. This can be used for random access decryption.
        //
        virtual Status CreateEncryptedDataStream(
            ILicense * license,
            IReadableStream * stream,
            const std::string & algorithm,
            IEncryptedStream ** encStream
            ) = 0;

        //
        // Registers the given User Key in the storage provider. Should be used
        // to pre-fill the User Keys provided by an external service. Optional
        // provider and user identifiers can be given to optimize the validation
        // of a license.
        //
        virtual Status AddUserKey(const std::string & userKey) = 0;
        virtual Status AddUserKey(
            const std::string & userKey,
            const std::string & userId,
            const std::string & providerId
            ) = 0;
        virtual Status AddUserKey(
            const std::string & userKey,
            const std::string & userId,
            const std::string & providerId,
            const std::string & licenseId
            ) = 0;

        //
        // Creates a new instance of IAcquisition to download the publication
        // of the given License from its Content Provider. The License Document
        // will be put inside the publication after the download is completed.
        // The given publication path is used for the download destination.
        // The IAcquisition is idle after its creation, you must start the
        // download explicitely with IAcquisition::Start().
        //
        virtual Status CreatePublicationAcquisition(
                const std::string & publicationPath,
                ILicense * license,
                IAcquisition ** acquisition
        ) = 0;

        virtual Status CreatePublicationStatusDocumentProcessing(
                const std::string & publicationPath,
                ILicense * license,
                IStatusDocumentProcessing ** statusDocumentProcessing
        ) = 0;


        //
        // Returns the rights service, exposing the public License rights API.
        //
        virtual IRightsService * GetRightsService() const = 0;

        virtual ~ILcpService() {}
    };
}

#endif //__I_LCP_SERVICE_H__
