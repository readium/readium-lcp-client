//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#ifndef __CRL_DISTRIBUTION_POINTS_H__
#define __CRL_DISTRIBUTION_POINTS_H__

#include "ICertificate.h"

namespace lcp
{
    class CrlDistributionPoints : public ICrlDistributionPoints
    {
    public:
        explicit CrlDistributionPoints(ICertificateExtension * extension);
        virtual bool HasCrlDistributionPoints() const;
        virtual const StringsList & CrlDistributionPointUrls() const;

    private:
        StringsList m_crlDistributionPointUrls;
    };
}

#endif  //__CRL_DISTRIBUTION_POINTS_H__
