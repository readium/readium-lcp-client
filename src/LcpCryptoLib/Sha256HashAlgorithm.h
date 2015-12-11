//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#ifndef __SHA256_HASH_ALGORITHM_H__
#define __SHA256_HASH_ALGORITHM_H__

#include <cryptopp/sha.h>
#include "CryptoAlgorithmInterfaces.h"

namespace lcp
{
    class Sha256HashAlgorithm : public IHashAlgorithm
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
