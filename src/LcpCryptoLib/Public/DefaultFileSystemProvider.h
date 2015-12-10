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
