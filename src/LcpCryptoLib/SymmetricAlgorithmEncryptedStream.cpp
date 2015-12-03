#include <cryptopp/cryptlib.h>
#include "SymmetricAlgorithmEncryptedStream.h"
#include "DecryptionContextImpl.h"

namespace lcp
{
    SymmetricAlgorithmEncryptedStream::SymmetricAlgorithmEncryptedStream(
        IReadableStream * stream,
        std::unique_ptr<ISymmetricAlgorithm> algorithm
        )
        : m_stream(stream)
        , m_algorithm(std::move(algorithm))
    {
    }

    int64_t SymmetricAlgorithmEncryptedStream::DecryptedSize()
    {
        try
        {
            return m_algorithm->PlainTextSize(m_stream);;
        }
        catch (const CryptoPP::Exception & ex)
        {
            throw std::runtime_error(ex.GetWhat());
        }
    }

    void SymmetricAlgorithmEncryptedStream::Read(unsigned char * pBuffer, int64_t sizeToRead)
    {
        try
        {
            DecryptionContextImpl context;
            context.SetDecryptionRange(static_cast<size_t>(m_readPosition), static_cast<size_t>(sizeToRead));
            m_algorithm->Decrypt(&context, m_stream, pBuffer, static_cast<size_t>(sizeToRead));
            m_readPosition += sizeToRead;
        }
        catch (const CryptoPP::Exception & ex)
        {
            throw std::runtime_error(ex.GetWhat());
        }
    }

    void SymmetricAlgorithmEncryptedStream::SetReadPosition(int64_t pos)
    {
        m_readPosition = pos;
    }

    int64_t SymmetricAlgorithmEncryptedStream::ReadPosition() const
    {
        return m_readPosition;
    }

    int64_t SymmetricAlgorithmEncryptedStream::Size()
    {
        return m_stream->Size();
    }
}