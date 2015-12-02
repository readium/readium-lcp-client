#ifndef __I_FILE_SYSTEM_H__
#define __I_FILE_SYSTEM_H__

#include <string>
#include "StreamInterfaces.h"

namespace lcp
{
    class IFile : public IReadableStream, public IWritableStream
    {
    public:
        virtual std::string GetPath() const = 0;
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
