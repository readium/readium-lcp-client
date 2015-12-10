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

#ifndef __DOWNLOAD_IN_MEMORY_REQUEST_H__
#define __DOWNLOAD_IN_MEMORY_REQUEST_H__

#include "BaseDownloadRequest.h"

namespace lcp
{
    class DownloadInMemoryRequest : public BaseDownloadRequest
    {
    public:
        DownloadInMemoryRequest(const std::string & url, IWritableStream * stream)
            : BaseDownloadRequest(url)
            , m_stream(stream)
        {
        }

        virtual IWritableStream * DestinationStream() const
        {
            return m_stream;
        }

        virtual bool HasDesinationPath() const
        {
            return false;
        }

        virtual std::string DestinationPath() const
        {
            return std::string();
        }

    private:
        IWritableStream * m_stream;
    };
}

#endif //__DOWNLOAD_IN_MEMORY_REQUEST_H__
