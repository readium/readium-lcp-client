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
#ifndef __BASE_DOWNLOAD_REQUEST_H__
#define __BASE_DOWNLOAD_REQUEST_H__

#include "Public/INetProvider.h"

namespace lcp
{
    class BaseDownloadRequest : public IDownloadRequest
    {
    public:
        BaseDownloadRequest(const std::string & url)
            : m_url(url)
            , m_canceled(false)
        {
        }

        virtual std::string Url() const
        {
            return m_url;
        }

        virtual bool Canceled() const
        {
            return m_canceled;
        }

        virtual void SetCanceled(bool value)
        {
            m_canceled = value;
        }

        virtual void SetSuggestedFileName(const std::string & fileName)
        {
            m_suggestedFileName = fileName;
        }

        virtual std::string SuggestedFileName() const
        {
            return m_suggestedFileName;
        }

    protected:
        bool m_canceled;
        std::string m_url;
        std::string m_suggestedFileName;
    };
}

#endif //__BASE_DOWNLOAD_REQUEST_H__
