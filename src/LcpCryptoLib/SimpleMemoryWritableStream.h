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

#ifndef __SIMPLE_MEMORY_WRITABLE_STREAM_H__
#define __SIMPLE_MEMORY_WRITABLE_STREAM_H__

#include <vector>
#include "Public/StreamInterfaces.h"

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
