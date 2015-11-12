#ifndef __LCP_SERVICE_H__
#define __LCP_SERVICE_H__

#include <map>
#include <memory>
#include "Public/ILcpService.h"

namespace lcp
{
    class JsonValueReader;

    class LcpService : public ILcpService
    {
    public:
        LcpService();

        // ILcpService
        virtual Status OpenLicense(const std::string & licenseJson, ILicense ** license);
        
    private:
        bool FindLicense(const std::string & canonicalJson, ILicense ** license);
        std::string CalculateCanonicalForm(const std::string & licenseJson);

    private:
        std::map<std::string, std::unique_ptr<ILicense> > m_licenses;
        std::unique_ptr<JsonValueReader> m_jsonReader;
    };
}
#endif //__LCP_SERVICE_H__
