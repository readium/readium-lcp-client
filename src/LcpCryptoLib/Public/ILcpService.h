#ifndef __I_LCP_SERVICE_H__
#define __I_LCP_SERVICE_H__

#include <string>
#include "LcpStatus.h"

namespace lcp
{
    class ILicense;
    class INetProvider;
    class IStorageProvider;

    class ILcpService
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

        // Decrypts and returns the given bytes using the given License.
        // Algorithm parameter may be dropped, in that the case algorithm 
        // from the Encryption Profile will be used.
        virtual Status DecryptData(
            ILicense * license,
            const unsigned char * data,
            const size_t dataLength,
            unsigned char * decryptedData,
            size_t * decryptedDataLength,
            const std::string & algorithm = ""
            ) = 0;

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

        virtual ~ILcpService() {}
    };
}

#endif //__I_LCP_SERVICE_H__
