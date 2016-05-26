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



#include "CrlDistributionPoints.h"
#include "CryptoppUtils.h"
#include "IncludeMacros.h"

CRYPTOPP_INCLUDE_START
#include <cryptopp/asn.h>
CRYPTOPP_INCLUDE_END

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