#ifndef __STREAM_INTERFACES_H__
#define __STREAM_INTERFACES_H__

namespace lcp
{
    class IReadableStream
    {
    public:
        virtual void Read(unsigned char * pBuffer, size_t sizeToRead) = 0;
        virtual void SetReadPosition(size_t pos) = 0;
        virtual size_t ReadPosition() const = 0;
        virtual size_t Size() = 0;
        virtual ~IReadableStream() {}
    };

    class IWritableStream
    {
    public:
        virtual void Write(const unsigned char * pBuffer, size_t sizeToWrite) = 0;
        virtual void SetWritePosition(size_t pos) = 0;
        virtual size_t WritePosition() const = 0;
        virtual ~IWritableStream() {}
    };

    class IEncryptedStream : public IReadableStream
    {
    public:
        virtual size_t DecryptedSize() = 0;
        virtual ~IEncryptedStream() {}
    };
}

#endif //__STREAM_INTERFACES_H__
