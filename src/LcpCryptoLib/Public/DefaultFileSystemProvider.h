#ifndef __DEFAULT_FILE_SYSTEM_PROVIDER_H__
#define __DEFAULT_FILE_SYSTEM_PROVIDER_H__

#include <fstream>
#include <sstream>
#include <errno.h>
#include "IFileSystemProvider.h"

namespace lcp
{
    class DefaultFile : public IFile
    {
    public:
        explicit DefaultFile(const std::string & path)
            : m_path(path)
        {
            m_fstream.open(path, std::ios::in | std::ios::out | std::fstream::trunc | std::ios::binary);
            if (!m_fstream.is_open())
            {
                std::stringstream strm;
                strm << "Can not open file: " << path << "; " << std::strerror(errno);
                throw std::runtime_error(strm.str());
            }
        }

        virtual std::string GetPath() const
        {
            return m_path;
        }

        virtual size_t Write(size_t position, const void * pBuffer, size_t sizeToWrite)
        {
            m_fstream.seekg(position);
            m_fstream.write(static_cast<const char *>(pBuffer), sizeToWrite);
            if (m_fstream.bad() || m_fstream.fail())
            {
                return 0;
            }
            return sizeToWrite;
        }

        virtual size_t Read(size_t position, void * pBuffer, size_t sizeToRead)
        {
            m_fstream.seekg(position);
            m_fstream.read(static_cast<char *>(pBuffer), sizeToRead);
            if (m_fstream.bad() || m_fstream.fail())
            {
                return 0;
            }
            return sizeToRead;
        }

        virtual size_t GetSize()
        {
            m_fstream.seekg(0, std::ios::end);
            return m_fstream.tellg();
        }

        ~DefaultFile()
        {
            m_fstream.close();
        }

    private:
        std::string m_path;
        std::fstream m_fstream;
    };

    class DefaultFileSystemProvider : public IFileSystemProvider
    {
    public:
        DefaultFileSystemProvider()
        {
        }
        virtual IFile * GetFile(const std::string & path)
        {
            return new DefaultFile(path);
        }
    };
}

#endif //__DEFAULT_FILE_SYSTEM_PROVIDER_H__
