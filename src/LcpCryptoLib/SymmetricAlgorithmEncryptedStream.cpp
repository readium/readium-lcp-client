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