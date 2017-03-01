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

#ifndef __I_LCP_SERVICE_H__
#define __I_LCP_SERVICE_H__

#include <string>
#include "LcpStatus.h"

namespace lcp
{
    class ILicense;
#if !DISABLE_NET_PROVIDER
    class INetProvider;
#endif //!DISABLE_NET_PROVIDER
    class IStorageProvider;
    class IFileSystemProvider;
#if ENABLE_NET_PROVIDER_ACQUISITION
    class IAcquisition;
    class IAcquisitionCallback;
#endif //ENABLE_NET_PROVIDER_ACQUISITION
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

#if !DISABLE_NET_PROVIDER
        //
        // Shared implementation of the net provider, used for any network
        // call from the library.
        //
        virtual INetProvider * NetProvider() const = 0;
#endif //!DISABLE_NET_PROVIDER

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
                const std::string & publicationPath,
                const std::string & licenseJson,
                ILicense** license) = 0;

        virtual Status InjectLicense(
                const std::string & publicationPath,
                const std::string & licenseJson) = 0;

        virtual Status InjectLicense(
                const std::string & publicationPath,
                ILicense * license) = 0;

        virtual int TimeStampCompare(const std::string & t1, const std::string & t2) = 0;

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

#if ENABLE_NET_PROVIDER_ACQUISITION
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
#endif //ENABLE_NET_PROVIDER_ACQUISITION

        //
        // Returns the rights service, exposing the public License rights API.
        //
        virtual IRightsService * GetRightsService() const = 0;

        virtual ~ILcpService() {}
    };
}

#endif //__I_LCP_SERVICE_H__
