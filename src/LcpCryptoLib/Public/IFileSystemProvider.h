//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#ifndef __I_FILE_SYSTEM_H__
#define __I_FILE_SYSTEM_H__

#include <string>
#include "StreamInterfaces.h"

namespace lcp
{
    class IFile : public IReadableStream, public IWritableStream
    {
    public:
        virtual std::string Path() const = 0;
        virtual ~IFile() {}
    };

    class IFileSystemProvider
    {
    public:
        enum OpenMode
        {
            CreateNew,
            ReadOnly,
        };

    public:
        virtual IFile * GetFile(const std::string & path, OpenMode openMode = CreateNew) = 0;
        virtual ~IFileSystemProvider() {}
    };
}

#endif //__I_FILE_SYSTEM_H__
