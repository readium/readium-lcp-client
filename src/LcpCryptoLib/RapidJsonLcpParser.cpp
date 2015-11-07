#include <memory>

#include "rapidjson/document.h"
#include "RapidJsonLcpParser.h"

#include "JsonValueReader.h"
#include "CryptoLcpNode.h"
#include "LinksLcpNode.h"
#include "UserLcpNode.h"
#include "RightsLcpNode.h"
#include "RootLcpNode.h"

namespace lcp
{
    Status RapidJsonLcpParser::Parse(const std::string & licenseJson, ILicense ** license)
    {
        rapidjson::Document document;
        //TODO: kParseValidateEncodingFlag
        if (document.Parse(licenseJson.c_str()).HasParseError())
        {
            return JsonValueReader::CreateRapidJsonError(document.GetParseError(), document.GetErrorOffset());
        }

        JsonValueReader reader;
        std::unique_ptr<CryptoLcpNode> cryptoNode(new CryptoLcpNode());
        std::unique_ptr<LinksLcpNode> linksNode(new LinksLcpNode());
        std::unique_ptr<UserLcpNode> userNode(new UserLcpNode());
        std::unique_ptr<RightsLcpNode> rightsNode(new RightsLcpNode());
        std::unique_ptr<RootLcpNode> rootNode(new RootLcpNode(cryptoNode.get(), linksNode.get(), nullptr, nullptr));//TODO: own

        rootNode->AddChildNode(cryptoNode.get());
        rootNode->AddChildNode(linksNode.get());
        rootNode->AddChildNode(userNode.get());
        rootNode->AddChildNode(rightsNode.get());
        Status res = rootNode->ParseNode(document, &reader);
        
        if (!Status::IsSuccess(res))
        {
            *license = nullptr;
            return res;
        }
        *license = rootNode.get();
        rootNode.release();
        return Status(StCodeCover::ErrorSuccess);
    }
}