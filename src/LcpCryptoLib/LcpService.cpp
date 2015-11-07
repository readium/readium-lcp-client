#include "LcpService.h"
#include "LcpUtils.h"

#include "RapidJsonLcpParser.h"

namespace lcp
{
    LcpService::LcpService()
    {
        m_jsonParser.reset(new RapidJsonLcpParser());
    }
    Status LcpService::OpenLicense(const std::string & licenseJSON, ILicense ** license)
    {
        try
        {
            return m_jsonParser->Parse(licenseJSON, license);
        }
        catch (const StatusException & ex)
        {
            return ex.ResultStatus();
        }
    }
}