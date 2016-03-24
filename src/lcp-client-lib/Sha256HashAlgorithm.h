//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#ifndef __SHA256_HASH_ALGORITHM_H__
#define __SHA256_HASH_ALGORITHM_H__

#include "IncludeMacros.h"
#include "CryptoAlgorithmInterfaces.h"
#include "NonCopyable.h"

CRYPTOPP_INCLUDE_START
#include <cryptopp/sha.h>
CRYPTOPP_INCLUDE_END

namespace lcp
{
    class Sha256HashAlgorithm : public IHashAlgorithm, public NonCopyable
    {
    public:
        virtual std::string Name() const;
        virtual size_t DigestSize() const;
        virtual void UpdateHash(const std::string & dataStr);
        virtual void UpdateHash(const unsigned char * data, const size_t dataLength);
        virtual KeyType Hash();

    private:
        CryptoPP::SHA256 m_sha256;
    };
}

#endif //__SHA256_HASH_ALGORITHM_H__
