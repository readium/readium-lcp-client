#ifndef __LCP_1DOT0_ENCRTYPTION_PROFILE_H__
#define __LCP_1DOT0_ENCRTYPTION_PROFILE_H__

#include "IEncryptionProfile.h"
#include "AlgorithmNames.h"

namespace lcp
{
    class Lcp1dot0EncryptionProfile : public IEncryptionProfile
    {
    public:
        std::string Self() const
        {
            return "http://readium.org/lcp/profile-1.0";
        }
        std::string PublicationAlgorithm() const
        {
            return AlgorithmNames::AesCbc256Id;
        }
        std::string ContentKeyAlgorithm() const
        {
            return AlgorithmNames::AesCbc256Id;
        }
        std::string UserKeyAlgorithm() const
        {
            return AlgorithmNames::Sha256Id;
        }
        std::string SignatureAlgorithm() const
        {
            return AlgorithmNames::RsaSha256Id;
        }
    };
}

#endif //__LCP_1DOT0_ENCRTYPTION_PROFILE_H__
