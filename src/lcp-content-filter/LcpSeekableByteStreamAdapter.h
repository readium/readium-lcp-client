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
#ifndef __LCP_BYTE_STREAM_ADAPTER_H__
#define __LCP_BYTE_STREAM_ADAPTER_H__

#include "IncludeMacros.h"
#include "StreamInterfaces.h"

READIUM_INCLUDE_START
#include <ePub3/utilities/byte_stream.h>
READIUM_INCLUDE_END

namespace lcp
{
    class SeekableByteStreamAdapter : public IReadableStream
    {
    public:
        SeekableByteStreamAdapter(ePub3::SeekableByteStream *byteStream) : m_byteStream(byteStream) {}
        
        virtual void Read(unsigned char * pBuffer, int64_t sizeToRead)
        {
            m_byteStream->ReadBytes(pBuffer, sizeToRead);
        }
        
        virtual void SetReadPosition(int64_t pos)
        {
            m_byteStream->Seek(pos, std::ios::beg);
        }
        
        virtual int64_t ReadPosition() const
        {
            return m_byteStream->Position();
        }
        
        virtual int64_t Size()
        {
            return m_byteStream->Position() + m_byteStream->BytesAvailable();
        }
        
    protected:
        ePub3::SeekableByteStream *m_byteStream;
    };
}

#endif // __LCP_BYTE_STREAM_ADAPTER_H__
