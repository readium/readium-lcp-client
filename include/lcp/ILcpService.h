#ifndef __I_LCP_SERVICE_H__
#define __I_LCP_SERVICE_H__

#include <string>
#include "LcpStatus.h"

namespace lcp
{
    class ILicense;

    class ILcpService
    {
    public:
        // Parses the given JSON License Document and returns a matching
        // License instance. The License will be automatically decrypted
        // if a valid User Key can be found in the storage provider.
        virtual Status OpenLicense(const std::string & licenseJson, ILicense ** license) = 0;

        // Root certificate provided by the Readium foundation to the Reader System
        virtual std::string RootCertificate() const = 0;
        virtual ~ILcpService() {}
    };
}

#endif //__I_LCP_SERVICE_H__
