//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#ifndef __LCP_1DOT0_ENCRTYPTION_PROFILE_H__
#define __LCP_1DOT0_ENCRTYPTION_PROFILE_H__

#include <memory>
#include "IEncryptionProfile.h"

namespace lcp
{
    class Lcp1dot0EncryptionProfile : public IEncryptionProfile
    {
    public:
        virtual std::string Name() const;
        virtual std::string UserKeyAlgorithm() const;
        virtual std::string PublicationAlgorithm() const;
        virtual std::string ContentKeyAlgorithm() const;
        virtual std::string SignatureAlgorithm() const;
        virtual IHashAlgorithm * CreateUserKeyAlgorithm() const;
        virtual ISymmetricAlgorithm * CreatePublicationAlgorithm(const KeyType & symmetricKey) const;
        virtual ISymmetricAlgorithm * CreateContentKeyAlgorithm(const KeyType & symmetricKey) const;
        virtual ISignatureAlgorithm * CreateSignatureAlgorithm(const KeyType & publicKey) const;
    };
}

#endif //__LCP_1DOT0_ENCRTYPTION_PROFILE_H__
