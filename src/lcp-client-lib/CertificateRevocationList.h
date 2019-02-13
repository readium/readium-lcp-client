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


#ifndef __CERTIFICATE_REVOCATION_LIST_H__
#define __CERTIFICATE_REVOCATION_LIST_H__

#if !DISABLE_CRL

#include <mutex>
#include "ICertificate.h"
#include "NonCopyable.h"

namespace lcp
{
    class CertificateRevocationList : public ICertificateRevocationList, public NonCopyable
    {
    public:
        CertificateRevocationList() = default;
//        explicit CertificateRevocationList(const Buffer & crlRaw);
        
        // ICertificateRevocationList
        virtual void UpdateRevocationList(const Buffer & crlRaw);
        virtual bool HasThisUpdateDate() const;
        virtual std::string ThisUpdateDate() const;
        virtual bool HasNextUpdateDate() const;
        virtual std::string NextUpdateDate() const;
        virtual bool SerialNumberRevoked(const std::string & serialNumber) const;
        virtual const StringsSet & RevokedSerialNumbers() const;
        virtual const void InsertRevokedSerialNumber(std::string serial);


    private:
        mutable std::mutex m_sync;
        std::string m_thisUpdate;
        std::string m_nextUpdate;
        StringsSet m_revokedSerialNumbers;
    };
}

#endif //!DISABLE_CRL

#endif //__CERTIFICATE_REVOCATION_LIST_H__
