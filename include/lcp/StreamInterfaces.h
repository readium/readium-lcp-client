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
