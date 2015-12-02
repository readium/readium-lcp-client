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
        virtual size_t DecryptedSize();

        // IReadableStream
        virtual void Read(unsigned char * pBuffer, size_t sizeToRead);
        virtual void SetReadPosition(size_t pos);
        virtual size_t ReadPosition() const;
        virtual size_t Size();

    private:
        size_t m_readPosition;
        IReadableStream * m_stream;
        std::unique_ptr<ISymmetricAlgorithm> m_algorithm;
    };
}

#endif //__SYMMETRIC_ALGORITHM_ENCRYPTED_STREAM_H__
