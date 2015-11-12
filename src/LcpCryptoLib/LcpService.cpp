#include "LcpService.h"
#include "CryptoLcpNode.h"
#include "LinksLcpNode.h"
#include "UserLcpNode.h"
#include "RightsLcpNode.h"
#include "RootLcpNode.h"
#include "JsonValueReader.h"
#include "JsonCanonicalizer.h"

namespace lcp
{
    LcpService::LcpService()
        : m_jsonReader(new JsonValueReader())
    {
    }

    Status LcpService::OpenLicense(const std::string & licenseJson, ILicense ** license)
    {
        if (license == nullptr)
        {
            throw std::invalid_argument("license is nullptr");
        }

        try
        {
            if (this->FindLicense(licenseJson, license))
            {
                return Status(Status(StCodeCover::ErrorCommonSuccess));
            }

            std::unique_ptr<CryptoLcpNode> cryptoNode(new CryptoLcpNode());
            std::unique_ptr<LinksLcpNode> linksNode(new LinksLcpNode());
            std::unique_ptr<UserLcpNode> userNode(new UserLcpNode());
            std::unique_ptr<RightsLcpNode> rightsNode(new RightsLcpNode());
            std::unique_ptr<RootLcpNode> rootNode(new RootLcpNode(
                licenseJson,
                cryptoNode.get(),
                linksNode.get(),
                userNode.get(),
                rightsNode.get())
                );

            rootNode->AddChildNode(std::move(cryptoNode));
            rootNode->AddChildNode(std::move(linksNode));
            rootNode->AddChildNode(std::move(userNode));
            rootNode->AddChildNode(std::move(rightsNode));

            auto parentValue = rapidjson::Value(rapidjson::kNullType);
            rootNode->ParseNode(parentValue, m_jsonReader.get());

            auto res = m_licenses.insert(std::make_pair(rootNode->Id(), std::move(rootNode)));
            if (!res.second)
            {
                return Status(StCodeCover::ErrorOpeningDuplicateLicense);
            }
            *license = rootNode.get();
            return Status(StCodeCover::ErrorCommonSuccess);
        }
        catch (const StatusException & ex)
        {
            return ex.ResultStatus();
        }
    }

    bool LcpService::FindLicense(const std::string & licenseJson, ILicense ** license)
    {
        JsonCanonicalizer canonicalizer(licenseJson, m_jsonReader.get());
        auto licIt = m_licenses.find(canonicalizer.Id());
        if (licIt != m_licenses.end())
        {
            std::string canonicalJson = canonicalizer.CanonicalLicense();
            if (canonicalJson == licIt->second->Content())
            {
                *license = licIt->second.get();
                return true;
            }
        }
        return false;
    }
}