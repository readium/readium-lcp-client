#ifndef __I_FILE_SYSTEM_H__
#define __I_FILE_SYSTEM_H__

#include <string>

namespace lcp
{
    class IFile
    {
    public:
        virtual std::string GetPath() const = 0;
        virtual size_t Write(const void * pBuffer, size_t sizeToWrite) = 0;
        virtual size_t Read(void * pBuffer, size_t sizeToRead) = 0;
        virtual ~IFile() {}
    };

    class IFileSystemProvider
    {
    public:
        virtual IFile * GetFile(const std::string & path) = 0;
        virtual ~IFileSystemProvider() {}
    };
}

#endif //__I_FILE_SYSTEM_H__
