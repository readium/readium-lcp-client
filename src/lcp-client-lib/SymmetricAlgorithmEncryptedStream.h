//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#ifndef __SYMMETRIC_ALGORITHM_ENCRYPTED_STREAM_H__
#define __SYMMETRIC_ALGORITHM_ENCRYPTED_STREAM_H__

#include <memory>
#include "public/StreamInterfaces.h"
#include "CryptoAlgorithmInterfaces.h"
#include "NonCopyable.h"

namespace lcp
{
    class SymmetricAlgorithmEncryptedStream : public IEncryptedStream, public NonCopyable
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
