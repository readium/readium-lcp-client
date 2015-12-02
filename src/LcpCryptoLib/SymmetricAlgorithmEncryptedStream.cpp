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
    {
        m_algorithm = std::move(algorithm);
    }

    size_t SymmetricAlgorithmEncryptedStream::DecryptedSize()
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

    void SymmetricAlgorithmEncryptedStream::Read(unsigned char * pBuffer, size_t sizeToRead)
    {
        try
        {
            DecryptionContextImpl context;
            context.SetDecryptionRange(m_readPosition, sizeToRead);
            m_algorithm->Decrypt(&context, m_stream, pBuffer, sizeToRead);
        }
        catch (const CryptoPP::Exception & ex)
        {
            throw std::runtime_error(ex.GetWhat());
        }
    }

    void SymmetricAlgorithmEncryptedStream::SetReadPosition(size_t pos)
    {
        m_readPosition = pos;
    }

    size_t SymmetricAlgorithmEncryptedStream::ReadPosition() const
    {
        return m_readPosition;
    }

    size_t SymmetricAlgorithmEncryptedStream::Size()
    {
        return m_stream->Size();
    }
}