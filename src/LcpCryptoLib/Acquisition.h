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

#ifndef __LCP_ACQUISITION_H__
#define __LCP_ACQUISITION_H__

#include <string>
#include "Public/IAcquistion.h"
#include "Public/IAcquistionCallback.h"
#include "Public/INetProvider.h"
#include "Public/IFileSystemProvider.h"
#include "Public/ILinks.h"

namespace lcp
{
    class ILicense;
    class ICryptoProvider;

    class Acquisition : public IAcquisition, public INetProviderCallback
    {
    public:
        Acquisition(
            ILicense * license,
            IFileSystemProvider * fileSystemProvider,
            INetProvider * netProvider,
            ICryptoProvider * cryptoProvider,
            const std::string & publicationPath
            );

        virtual Status Start(IAcquisitionCallback * callback);
        virtual void Cancel();
        virtual std::string PublicationPath() const;
        virtual std::string SuggestedFileName() const;

    public:
        // INetProviderCallback
        virtual void OnRequestStarted(INetRequest * request);
        virtual void OnRequestProgressed(INetRequest * request, float progress);
        virtual void OnRequestCanceled(INetRequest * request);
        virtual void OnRequestEnded(INetRequest * request, Status result);

    public:
        static const char * PublicationType;

    private:
        Status CheckPublicationHash(const Link & link);

    private:
        ILicense * m_license;
        IFileSystemProvider * m_fileSystemProvider;
        INetProvider * m_netProvider;
        ICryptoProvider * m_cryptoProvider;
        std::string m_publicationPath;

        Link m_publicationLink;
        IAcquisitionCallback * m_callback;
        std::unique_ptr<IFile> m_file;
        std::unique_ptr<IDownloadRequest> m_request;
    };
}

#endif //__LCP_ACQUISITION_H__
