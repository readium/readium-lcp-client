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

#ifndef __SYMMETRIC_ALGORITHM_ENCRYPTED_STREAM_H__
#define __SYMMETRIC_ALGORITHM_ENCRYPTED_STREAM_H__

#include <memory>
#include "Public/StreamInterfaces.h"
#include "CryptoAlgorithmInterfaces.h"

namespace lcp
{
    class SymmetricAlgorithmEncryptedStream : public IEncryptedStream
    {
    public:
        SymmetricAlgorithmEncryptedStream(
            IReadableStream * stream,
            std::unique_ptr<ISymmetricAlgorithm> algorithm
            );

        // IEncryptedStream
        virtual int64_t DecryptedSize();

        // IReadableStream
        virtual void Read(unsigned char * pBuffer, int64_t sizeToRead);
        virtual void SetReadPosition(int64_t pos);
        virtual int64_t ReadPosition() const;
        virtual int64_t Size();

    private:
        int64_t m_readPosition;
        IReadableStream * m_stream;
        std::unique_ptr<ISymmetricAlgorithm> m_algorithm;
    };
}

#endif //__SYMMETRIC_ALGORITHM_ENCRYPTED_STREAM_H__
