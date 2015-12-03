#ifndef __STREAM_INTERFACES_H__
#define __STREAM_INTERFACES_H__

#include <cstdlib>

namespace lcp
{
    class IReadableStream
    {
    public:
        virtual void Read(unsigned char * pBuffer, int64_t sizeToRead) = 0;
        virtual void SetReadPosition(int64_t pos) = 0;
        virtual int64_t ReadPosition() const = 0;
        virtual int64_t Size() = 0;
        virtual ~IReadableStream() {}
    };

    class IWritableStream
    {
    public:
        virtual void Write(const unsigned char * pBuffer, int64_t sizeToWrite) = 0;
        virtual void SetWritePosition(int64_t pos) = 0;
        virtual int64_t WritePosition() const = 0;
        virtual ~IWritableStream() {}
    };

    class IEncryptedStream : public IReadableStream
    {
    public:
        virtual int64_t DecryptedSize() = 0;
        virtual ~IEncryptedStream() {}
    };
}

#endif //__STREAM_INTERFACES_H__
