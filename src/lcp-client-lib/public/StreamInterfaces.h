//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#ifndef __STREAM_INTERFACES_H__
#define __STREAM_INTERFACES_H__

#include <cstdlib>

namespace lcp
{
    //
    // Interface to read a stream of data.
    //
    class IReadableStream
    {
    public:
        virtual void Read(unsigned char * pBuffer, int64_t sizeToRead) = 0;
        virtual void SetReadPosition(int64_t pos) = 0;
        virtual int64_t ReadPosition() const = 0;
        virtual int64_t Size() = 0;
        virtual ~IReadableStream() {}
    };

    //
    // Interface to write data to a stream.
    //
    class IWritableStream
    {
    public:
        virtual void Write(const unsigned char * pBuffer, int64_t sizeToWrite) = 0;
        virtual void SetWritePosition(int64_t pos) = 0;
        virtual int64_t WritePosition() const = 0;
        virtual ~IWritableStream() {}
    };

    //
    // A read-only stream that will decrypt its data on-the-fly when reading it.
    // You must not read more than DecryptedSize data from this stream.
    //
    class IEncryptedStream : public IReadableStream
    {
    public:
        virtual int64_t DecryptedSize() = 0;
        virtual ~IEncryptedStream() {}
    };
}

#endif //__STREAM_INTERFACES_H__
