// Copyright (c) 2016 Mantano
// Licensed to the Readium Foundation under one or more contributor license agreements.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation and/or
//    other materials provided with the distribution.
// 3. Neither the name of the organization nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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

        DefaultFile(const DefaultFile &) = delete;
        DefaultFile & operator=(const DefaultFile &) = delete;

        virtual std::string Path() const
        {
            return m_path;
        }

        virtual void SetReadPosition(int64_t pos)
        {
            m_readPosition = pos;
            this->InternalSetPosition(pos);
        }

        virtual int64_t ReadPosition() const
        {
            return m_readPosition;
        }

        virtual void SetWritePosition(int64_t pos)
        {
            m_writePosition = pos;
            this->InternalSetPosition(pos);
        }

        virtual int64_t WritePosition() const
        {
            return m_writePosition;
        }

        virtual void Write(const unsigned char * pBuffer, int64_t sizeToWrite)
        {
            int64_t curPos = m_fstream.tellg();
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

        virtual void Read(unsigned char * pBuffer, int64_t sizeToRead)
        {
            int64_t curPos = m_fstream.tellg();
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

        virtual int64_t Size()
        {
            m_fstream.seekg(0, std::ios::end);
            return m_fstream.tellg();
        }

        ~DefaultFile()
        {
            m_fstream.close();
        }

    private:
        void InternalSetPosition(int64_t pos)
        {
            m_fstream.seekg(pos, std::ios::beg);
            if (m_fstream.bad() || m_fstream.fail() || m_fstream.eof())
            {
                std::stringstream strm;
                strm << "Can not seek to position: " << pos << " " << m_path << "; " << std::strerror(errno);
                throw std::runtime_error(strm.str());
            }
        }

        void ThrowError(const std::string & error)
        {
            std::stringstream strm;
            strm << error << m_path << "; " << std::strerror(errno);
            throw std::runtime_error(strm.str());
        }

    private:
        std::string m_path;
        std::fstream m_fstream;
        int64_t m_writePosition;
        int64_t m_readPosition;
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
