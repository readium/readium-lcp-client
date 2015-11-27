#ifndef __DEFAULT_FILE_SYSTEM_PROVIDER_H__
#define __DEFAULT_FILE_SYSTEM_PROVIDER_H__

#include "Public/IFileSystemProvider.h"

namespace lcp
{
    class DefaultFile : public IFile
    {
    public:
        explicit DefaultFile(const std::string & path)
            : m_path(path)
        {
        }

        virtual std::string GetPath() const
        {
            return m_path;
        }
        virtual size_t Write(const void * pBuffer, size_t sizeToWrite)
        {
            return 0;
        }
        virtual size_t Read(void * pBuffer, size_t sizeToRead)
        {
            return 0;
        }

    private:
        std::string m_path;
    };

    class DefaultFileSystemProvider : public IFileSystemProvider
    {
    public:
        DefaultFileSystemProvider();
        virtual IFile * GetFile(const std::string & path)
        {
            return new DefaultFile(path);
        }
    };
}

#endif //__DEFAULT_FILE_SYSTEM_PROVIDER_H__
