//
//  Created by Mickaël Menu on 02/12/15.
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

#ifndef __LCP_BYTE_STREAM_ADAPTER_H__
#define __LCP_BYTE_STREAM_ADAPTER_H__

#include "StreamInterfaces.h"
#include <ePub3/utilities/byte_stream.h>

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
