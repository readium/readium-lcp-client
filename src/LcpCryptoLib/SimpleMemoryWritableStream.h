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
            memcpy_s(&m_buffer.at(m_position), convSizeToWrite, pBuffer, convSizeToWrite);
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
