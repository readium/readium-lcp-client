#ifndef __LCP_SERVICE_H__
#define __LCP_SERVICE_H__

#include <memory>

#include "ILcpService.h"

#include "IJsonLcpParser.h"

namespace lcp
{
    class LcpService : public ILcpService
    {
    public:
        LcpService();
        virtual Status OpenLicense(const std::string & licenseJSON, ILicense ** license);

    private:
        std::unique_ptr<IJsonLcpParser> m_jsonParser;
    };
}
#endif //__LCP_SERVICE_H__
