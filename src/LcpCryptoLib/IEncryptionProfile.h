#ifndef __I_ENCRTYPTION_PROFILE_H__
#define __I_ENCRTYPTION_PROFILE_H__

#include <string>

namespace lcp
{
    class IEncryptionProfile
    {
    public:
        virtual std::string Self() const = 0;
        virtual std::string PublicationAlgorithm() const = 0;
        virtual std::string ContentKeyAlgorithm() const = 0;
        virtual std::string UserKeyAlgorithm() const = 0;
        virtual std::string SignatureAlgorithm() const = 0;
        virtual ~IEncryptionProfile() {}
    };
}

#endif //__I_ENCRTYPTION_PROFILE_H__
