//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//  Licensed under Gnu Affero General Public License Version 3 (provided, notwithstanding this notice,
//  Readium Foundation reserves the right to license this material under a different separate license,
//  and if you have done so, the terms of that separate license control and the following references
//  to GPL do not apply).
//
//  This program is free software: you can redistribute it and/or modify it under the terms of the GNU
//  Affero General Public License as published by the Free Software Foundation, either version 3 of
//  the License, or (at your option) any later version. You should have received a copy of the GNU
//  Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "AlgorithmNames.h"
#include "Sha256HashAlgorithm.h"

namespace lcp
{
    std::string Sha256HashAlgorithm::Name() const
    {
        return AlgorithmNames::Sha256Id;
    }

    size_t Sha256HashAlgorithm::DigestSize() const
    {
        return CryptoPP::SHA256::DIGESTSIZE;
    }

    void Sha256HashAlgorithm::UpdateHash(const std::string & dataStr)
    {
        this->UpdateHash(reinterpret_cast<const unsigned char *>(dataStr.data()), dataStr.size());
    }

    void Sha256HashAlgorithm::UpdateHash(const unsigned char * data, const size_t dataLength)
    {
        m_sha256.Update(data, dataLength);
    }

    KeyType Sha256HashAlgorithm::Hash()
    {
        KeyType hash(CryptoPP::SHA256::DIGESTSIZE);
        m_sha256.Final(hash.data());
        return hash;
    }
}