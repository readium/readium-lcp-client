#include <sstream>
#include "CertificateExtension.h"

namespace lcp
{
    CertificateExtension::CertificateExtension(CryptoPP::BERSequenceDecoder & parent)
    {
        this->Decode(parent);
    }

    CertificateExtension::CertificateExtension(const CryptoPP::OID & oid, const bool isCritical, const Buffer & value)
        : m_extensionId(oid), m_isCritical(isCritical), m_extensionValue(value)
    {
    }

    void CertificateExtension::Decode(CryptoPP::BERSequenceDecoder & parent)
    {
        CryptoPP::BERSequenceDecoder extension(parent);
        {
            m_extensionId.BERDecode(extension);
            m_isCritical = false;
            byte booleanTag = extension.PeekByte();
            if (booleanTag == CryptoPP::BOOLEAN)
            {
                CryptoPP::BERDecodeUnsigned(extension, m_isCritical, CryptoPP::BOOLEAN);
            }
            CryptoPP::SecByteBlock secBuffer;
            CryptoPP::BERDecodeOctetString(extension, secBuffer);
            m_extensionValue.assign(secBuffer.begin(), secBuffer.end());
        }
        extension.MessageEnd();
    }

    CryptoPP::OID CertificateExtension::CryptoOid() const
    {
        return m_extensionId;
    }

    std::string CertificateExtension::Oid() const
    {
        std::stringstream oid;
        for (auto val : m_extensionId.m_values)
        {
            oid << val << ".";
        }
        std::string oidStr = oid.str();
        return oidStr.substr(0, oidStr.find_last_of("."));
    }

    bool CertificateExtension::IsCritical() const
    {
        return m_isCritical;
    }

    const Buffer & CertificateExtension::Value() const
    {
        return m_extensionValue;
    }
}