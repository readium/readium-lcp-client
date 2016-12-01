// Copyright (c) 2016 Mantano
// Licensed to the Readium Foundation under one or more contributor license agreements.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation and/or
//    other materials provided with the distribution.
// 3. Neither the name of the organization nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#include "IncludeMacros.h"
#include "DecryptionContextImpl.h"
#include "SymmetricAlgorithmEncryptedStream.h"

CRYPTOPP_INCLUDE_START
#include <cryptopp/cryptlib.h>
CRYPTOPP_INCLUDE_END

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