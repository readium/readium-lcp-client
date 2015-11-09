#include "LcpService.h"
#include "CryptoLcpNode.h"
#include "LinksLcpNode.h"
#include "UserLcpNode.h"
#include "RightsLcpNode.h"
#include "RootLcpNode.h"
#include "JsonValueReader.h"

namespace lcp
{
    LcpService::LcpService()
    {
    }

    Status LcpService::OpenLicense(const std::string & licenseJSON, ILicense ** license)
    {
        if (license == nullptr)
        {
            throw std::invalid_argument("license is nullptr");
        }

        try
        {
            std::unique_ptr<CryptoLcpNode> cryptoNode(new CryptoLcpNode());
            std::unique_ptr<LinksLcpNode> linksNode(new LinksLcpNode());
            std::unique_ptr<UserLcpNode> userNode(new UserLcpNode());
            std::unique_ptr<RightsLcpNode> rightsNode(new RightsLcpNode());
            std::unique_ptr<RootLcpNode> rootNode(new RootLcpNode(
                licenseJSON,
                cryptoNode.get(),
                linksNode.get(),
                userNode.get(),
                rightsNode.get())
                );

            rootNode->AddChildNode(std::move(cryptoNode));
            rootNode->AddChildNode(std::move(linksNode));
            rootNode->AddChildNode(std::move(userNode));
            rootNode->AddChildNode(std::move(rightsNode));

            JsonValueReader reader;
            auto parentValue = rapidjson::Value(rapidjson::kNullType);
            rootNode->ParseNode(parentValue, &reader);

            auto status = Status(StCodeCover::ErrorCommonSuccess);
            *license = rootNode.get();
            rootNode.release();
            return status;
        }
        catch (const StatusException & ex)
        {
            return ex.ResultStatus();
        }
    }
}