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
            std::string canonicalJson = this->CalculateCanonicalForm(licenseJson);
            if (this->FindLicense(canonicalJson, license))
            {
                return Status(Status(StCodeCover::ErrorCommonSuccess));
            }

            std::unique_ptr<CryptoLcpNode> cryptoNode(new CryptoLcpNode());
            std::unique_ptr<LinksLcpNode> linksNode(new LinksLcpNode());
            std::unique_ptr<UserLcpNode> userNode(new UserLcpNode());
            std::unique_ptr<RightsLcpNode> rightsNode(new RightsLcpNode());
            std::unique_ptr<RootLcpNode> rootNode(new RootLcpNode(
                licenseJson,
                std::move(canonicalJson),
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

            auto insertRes = m_licenses.insert(std::make_pair(rootNode->Content(), std::move(rootNode)));
            if (!insertRes.second)
            {
                throw std::runtime_error("Two License instances with the same canonical form");
            }
            *license = insertRes.first->second.get();
            return Status(StCodeCover::ErrorCommonSuccess);
        }
        catch (const StatusException & ex)
        {
            return ex.ResultStatus();
        }
    }

    bool LcpService::FindLicense(const std::string & canonicalJson, ILicense ** license)
    {
        auto licIt = m_licenses.find(canonicalJson);
        if (licIt != m_licenses.end())
        {
            if (canonicalJson.size() == licIt->second->Content().size() &&
                std::equal(canonicalJson.begin(), canonicalJson.end(), licIt->second->Content().begin()))
            {
                *license = licIt->second.get();
                return true;
            }
        }
        return false;
    }

    std::string LcpService::CalculateCanonicalForm(const std::string & licenseJson)
    {
        JsonCanonicalizer canonicalizer(licenseJson, m_jsonReader.get());
        return canonicalizer.CanonicalLicense();
    }
}