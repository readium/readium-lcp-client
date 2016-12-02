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


#ifndef __SIMPLE_MEMORY_WRITABLE_STREAM_H__
#define __SIMPLE_MEMORY_WRITABLE_STREAM_H__

#include <vector>
#include "public/StreamInterfaces.h"

namespace lcp
{
    class SimpleMemoryWritableStream : public IWritableStream
    {
    public:
        explicit SimpleMemoryWritableStream(size_t size = DefaultBufferSize)
            : m_buffer(size)
            , m_position(0)
        {
        }

        const std::vector<unsigned char> & Buffer() const
        {
            return m_buffer;
        }

        virtual void Write(const unsigned char * pBuffer, int64_t sizeToWrite)
        {
            size_t convSizeToWrite = static_cast<size_t>(sizeToWrite);
            this->EnsureSize(m_position + convSizeToWrite);
            std::copy(pBuffer, pBuffer + convSizeToWrite, m_buffer.begin() + m_position);
        }

        virtual void SetWritePosition(int64_t pos)
        {
            size_t convPos = static_cast<size_t>(pos);
            this->EnsureSize(convPos);
            m_position = convPos;
        }

        virtual int64_t WritePosition() const
        {
            return m_position;
        }

    private:
        void EnsureSize(size_t newSize)
        {
            if (newSize > m_buffer.size())
            {
                m_buffer.resize(newSize);
            }
        }

    private:
        std::vector<unsigned char> m_buffer;
        size_t m_position;
        static const size_t DefaultBufferSize = 1024 * 8;
    };
}

#endif //__SIMPLE_MEMORY_WRITABLE_STREAM_H__
