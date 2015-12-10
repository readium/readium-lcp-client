//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#ifndef __I_ENCRTYPTION_PROFILE_H__
#define __I_ENCRTYPTION_PROFILE_H__

#include <string>
#include <vector>
#include "LcpUtils.h"

namespace lcp
{
    class ISymmetricAlgorithm;
    class IHashAlgorithm;
    class ISignatureAlgorithm;

    class IEncryptionProfile
    {
    public:
        virtual std::string Name() const = 0;
        virtual std::string UserKeyAlgorithm() const = 0;
        virtual std::string PublicationAlgorithm() const = 0;
        virtual std::string ContentKeyAlgorithm() const = 0;
        virtual std::string SignatureAlgorithm() const = 0;
        virtual IHashAlgorithm * CreateUserKeyAlgorithm() const = 0;
        virtual ISymmetricAlgorithm * CreatePublicationAlgorithm(const KeyType & symmetricKey) const = 0;
        virtual ISymmetricAlgorithm * CreateContentKeyAlgorithm(const KeyType & symmetricKey) const = 0;
        virtual ISignatureAlgorithm * CreateSignatureAlgorithm(const KeyType & publicKey) const = 0;
        virtual ~IEncryptionProfile() {}
    };
}

#endif //__I_ENCRTYPTION_PROFILE_H__
