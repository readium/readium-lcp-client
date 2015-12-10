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

#include <cryptopp/asn.h>
#include "CrlDistributionPoints.h"
#include "CryptoppUtils.h"

using namespace CryptoPP;

namespace lcp
{
    CrlDistributionPoints::CrlDistributionPoints(ICertificateExtension * extension)
    {
        ByteQueue distPointsQueue;
        distPointsQueue.Put(extension->Value().data(), extension->Value().size());
        distPointsQueue.MessageEnd();

        BERSequenceDecoder distributionPoints(distPointsQueue);
        {
            while (!distributionPoints.EndReached())
            {
                BERSequenceDecoder nextDp(distributionPoints);
                byte dpContext = nextDp.PeekByte();
                if (dpContext != CryptoppUtils::Cert::ContextSpecificTagZero)
                {
                    nextDp.SkipAll();
                    continue;
                }

                BERGeneralDecoder dpName(nextDp, CryptoppUtils::Cert::ContextSpecificTagZero);
                byte dpNameContext = dpName.PeekByte();
                if (dpNameContext != CryptoppUtils::Cert::ContextSpecificTagZero)
                {
                    dpName.SkipAll();
                    nextDp.SkipAll();
                    continue;
                }

                BERGeneralDecoder dpFullName(dpName, CryptoppUtils::Cert::ContextSpecificTagZero);
                {
                    byte dpFullNameContext = dpFullName.PeekByte();
                    if (dpFullNameContext == CryptoppUtils::Cert::ContextSpecificTagSixIA5String)
                    {
                        std::string url;
                        BERDecodeTextString(dpFullName, url, CryptoppUtils::Cert::ContextSpecificTagSixIA5String);
                        m_crlDistributionPointUrls.push_back(url);
                    }
                    dpName.SkipAll();
                    nextDp.SkipAll();
                }
            }
        }
        distributionPoints.MessageEnd();
    }

    bool CrlDistributionPoints::HasCrlDistributionPoints() const
    {
        return !m_crlDistributionPointUrls.empty();
    }

    const StringsList & CrlDistributionPoints::CrlDistributionPointUrls() const
    {
        return m_crlDistributionPointUrls;
    }
}