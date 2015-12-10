//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
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