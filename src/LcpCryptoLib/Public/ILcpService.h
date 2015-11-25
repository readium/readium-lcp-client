#ifndef __I_LCP_SERVICE_H__
#define __I_LCP_SERVICE_H__

#include <string>
#include "LcpStatus.h"

namespace lcp
{
    class ILicense;
    class INetProvider;
    class IStorageProvider;
    class IDecryptionContext;

    class IClientProvider
    {
    public:
        // Root certificate provided by the Readium foundation to the Reader System
        virtual std::string RootCertificate() const = 0;

        // Shared implementation of the net provider, used for any
        // network call from the library. To be set by the
        // library's client.
        virtual INetProvider * NetProvider() const = 0;

        // Shared implementation of the secure storage provider, used
        // to store and retrieve safely any JSON value by the library.
        // To be set by the library's client.
        virtual IStorageProvider * StorageProvider() const = 0;

        virtual ~IClientProvider() {}
    };

    class ILcpService : public IClientProvider
    {
    public:
        // Parses the given JSON License Document and returns a matching
        // License instance. The License will be automatically decrypted
        // if a valid User Key can be found in the storage provider.
        virtual Status OpenLicense(const std::string & licenseJson, ILicense ** license) = 0;

        // Decrypts the License Document using the given User Passphrase.
        // If the resulting User Key is valid, it will be permanently 
        // stored inside the storage provider for future retrieval.
        virtual Status DecryptLicense(ILicense * license, const std::string & userPassphrase) = 0;

        // Creates Decryption Context for multiple calls on the same message/stream/file
        virtual Status CreateDecryptionContext(IDecryptionContext ** decryptionContext) = 0;

        // Decrypts and returns the given bytes using the given License.
        // dataLength is the size of the encrypted data
        // decryptedDataLength will be set to the size of the decrypted
        // data. Algorithm parameter must be the same as in the
        // Encryption Profile. IDecryptionContext may be null if we need
        // to decrypt data in one shot
        virtual Status DecryptData(
            ILicense * license,
            IDecryptionContext * context,
            const unsigned char * data,
            const size_t dataLength,
            unsigned char * decryptedData,
            size_t inDecryptedDataLength,
            size_t * outDecryptedDataLength,
            const std::string & algorithm
            ) = 0;

        // Registers the given User Key in the storage provider. It can 
        // be used to pre-fill the User Keys provided by an external 
        // service. Optional provider and user identifiers can be given 
        // to optimize the validation of a license.
        virtual Status AddUserKey(const std::string & userKey) = 0;
        virtual Status AddUserKey(
            const std::string & userKey,
            const std::string & userId,
            const std::string & providerId
            ) = 0;

        virtual ~ILcpService() {}
    };
}

#endif //__I_LCP_SERVICE_H__
