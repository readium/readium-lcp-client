#include <vector>
#include <rapidjson/document.h>

#include "RapidJsonLcpParser.h"
#include "LcpUtils.h"

using namespace rapidjson;

namespace lcp
{
    class IValueInterperter
    {
    public:
        virtual void Interpret(const std::string & name, const Value & value) = 0;
        virtual ~IValueInterperter() {}
    };

    class ValueInterpreter
    {
    public:
        std::string InterpretAsString(const std::string & name, const Value & jsonValue)
        {
            Value::ConstMemberIterator it = jsonValue.FindMember(name.c_str());
            if (it != jsonValue.MemberEnd() && it->value.IsString())
            {
                return it->value.GetString();
            }
            return std::string();
        }

        const Value & InterpretAsObject(const std::string & name, const Value & jsonValue)
        {
            Value::ConstMemberIterator it = jsonValue.FindMember(name.c_str());
            if (it != jsonValue.MemberEnd() && it->value.IsObject())
            {
                return it->value;
            }
            return Value().Move();
        }
    };

    class ILcpStructureParser
    {
    public:
        virtual Status ProcessNode(const Value & jsonValue, ValueInterpreter * interpreter) = 0;
        virtual void Register(ILcpStructureParser * child) = 0;
        virtual ~ILcpStructureParser() {}
    };

    class BaseLcpStructureParser : public ILcpStructureParser
    {
    public:
        virtual void Register(ILcpStructureParser * child)
        {
            m_childs.push_back(child);
        }

    protected:
        std::vector<ILcpStructureParser *> m_childs;
    };

    Status CreateRapidJsonError(ParseErrorCode errorCode)
    {
        return CreateStatus(
            StatusCover::ErrorLicenseNotValid,
            ("RapidJson error code: " + std::to_string(static_cast<int>(errorCode))).c_str()
            );
    }

    class RootStructureParser : public BaseLcpStructureParser
    {
    public:
        virtual Status ProcessNode(const Value & parentValue, ValueInterpreter * interpreter)
        {
            if (!parentValue.IsObject())
            {
                return CreateRapidJsonError(kParseErrorValueInvalid);
            }

            std::string id = interpreter->InterpretAsString("id", parentValue);
            std::string issued = interpreter->InterpretAsString("issued", parentValue);
            std::string provider = interpreter->InterpretAsString("provider", parentValue);
            std::string updated = interpreter->InterpretAsString("updated", parentValue);

            if (id.empty() || issued.empty() || provider.empty())
            {
                return CreateStatus(StatusCover::ErrorLicenseNotValid, "globals of .lcpl are not valid");
            }

            for (auto it = m_childs.begin(); it != m_childs.end(); ++it)
            {
                Status res = (*it)->ProcessNode(parentValue, interpreter);
                if (!IsSuccess(res))
                {
                    return res;
                }
            }
            return CreateStatus(StatusCover::ErrorSuccess);
        }
    };

    class EncryptionStructureParser : public BaseLcpStructureParser
    {
    public:
        virtual Status ProcessNode(const Value & parentValue, ValueInterpreter * interpreter)
        {
            const Value & encryptionObj = interpreter->InterpretAsObject("encryption", parentValue);

            std::string contentKeyBase64 = interpreter->InterpretAsString("encrypted_value", encryptionObj);
            std::string contentAlgorithmUri = interpreter->InterpretAsString("algorithm", encryptionObj);

            return CreateStatus(StatusCover::ErrorSuccess);
        }
    };

    Status RapidJsonLcpParser::Parse(const std::string & licenseJson, std::unique_ptr<ILicense> & license)
    {
        Document document;
        //kParseValidateEncodingFlag
        if (document.Parse(licenseJson.c_str()).HasParseError())
        {
            return CreateRapidJsonError(document.GetParseError());
        }
        RootStructureParser rootParser;
        EncryptionStructureParser encryptionParser;
        ValueInterpreter interpreter;
        rootParser.Register(&encryptionParser);
        return rootParser.ProcessNode(document, &interpreter);
    }
}