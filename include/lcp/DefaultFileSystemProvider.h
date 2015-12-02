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
        explicit DefaultFile(const std::string & path, IFileSystemProvider::OpenMode openMode)
            : m_path(path)
            , m_readPosition(0)
            , m_writePosition(0)
        {
            int mode = 0;
            if (openMode == IFileSystemProvider::CreateNew)
                mode = std::ios::in | std::ios::out | std::fstream::trunc | std::ios::binary;
            else if (openMode == IFileSystemProvider::ReadOnly)
                mode = std::ios::in  | std::ios::binary;

            m_fstream.open(path, mode);
            if (!m_fstream.is_open())
            {
                this->ThrowError("Can not open file: ");
            }
        }

        virtual std::string GetPath() const
        {
            return m_path;
        }

        virtual void SetReadPosition(size_t pos)
        {
            m_readPosition = pos;
            this->InternalSetPosition(pos);
        }

        virtual size_t ReadPosition() const
        {
            return m_readPosition;
        }

        virtual void SetWritePosition(size_t pos)
        {
            m_writePosition = pos;
            this->InternalSetPosition(pos);
        }

        virtual size_t WritePosition() const
        {
            return m_writePosition;
        }

        virtual void Write(const unsigned char * pBuffer, size_t sizeToWrite)
        {
            size_t curPos = m_fstream.tellg();
            if (curPos != m_writePosition)
            {
                this->InternalSetPosition(m_writePosition);
            }

            m_fstream.write(reinterpret_cast<const char *>(pBuffer), sizeToWrite);
            if (m_fstream.bad() || m_fstream.fail())
            {
                this->ThrowError("Can not write to file: ");
            }

            m_writePosition += sizeToWrite;
            this->InternalSetPosition(m_writePosition);
        }

        virtual void Read(unsigned char * pBuffer, size_t sizeToRead)
        {
            long long curPos = m_fstream.tellg();
            if (curPos != m_readPosition)
            {
                this->InternalSetPosition(m_readPosition);
            }

            m_fstream.read(reinterpret_cast<char *>(pBuffer), sizeToRead);
            if (m_fstream.bad() || m_fstream.fail())
            {
                this->ThrowError("Can not read from file: ");
            }

            m_readPosition += sizeToRead;
            this->InternalSetPosition(m_readPosition);
        }

        virtual size_t Size()
        {
            m_fstream.seekg(0, std::ios::end);
            return m_fstream.tellg();
        }

        ~DefaultFile()
        {
            m_fstream.close();
        }

    private:
        void InternalSetPosition(size_t pos)
        {
            m_fstream.seekg(pos, std::ios::beg);
            if (m_fstream.bad() || m_fstream.fail() || m_fstream.eof())
            {
                this->ThrowError("Can not seek to position: ");
            }
        }

        void ThrowError(const std::string & error)
        {
            std::stringstream strm;
            strm << "Can not open file: " << m_path << "; " << std::strerror(errno);
            throw std::runtime_error(strm.str());
        }

    private:
        std::string m_path;
        std::fstream m_fstream;
        size_t m_writePosition;
        size_t m_readPosition;
    };

    class DefaultFileSystemProvider : public IFileSystemProvider
    {
    public:
        DefaultFileSystemProvider()
        {
        }
        virtual IFile * GetFile(const std::string & path, OpenMode openMode = CreateNew)
        {
            return new DefaultFile(path, openMode);
        }
    };
}

#endif //__DEFAULT_FILE_SYSTEM_PROVIDER_H__
