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
    class IFile;
    
    //
    // Provider used to read and write on the file system.
    // You can implement this interface to change the way files are accessed.
    // A default cross-platform C++ implementation is available with
    // DefaultFileSystemProvider.
    //
    class IFileSystemProvider
    {
    public:
        enum OpenMode
        {
            CreateNew,
            ReadOnly,
        };

    public:
        //
        // Opens a new IO stream to the given absolute path.
        //
        virtual IFile * GetFile(const std::string & path, OpenMode openMode = CreateNew) = 0;
        
        virtual ~IFileSystemProvider() {}
    };
    
    //
    // IO stream for a single fil on the file system.
    //
    class IFile : public IReadableStream, public IWritableStream
    {
    public:
        //
        // Returns the absolute path to the file.
        //
        virtual std::string GetPath() const = 0;
        
        virtual ~IFile() {}
    };
}

#endif //__I_FILE_SYSTEM_H__
