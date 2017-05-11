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


#ifndef __CRYPTOPP_CRYPTO_PROVIDER_H__
#define __CRYPTOPP_CRYPTO_PROVIDER_H__

#include <memory>
#include <mutex>
#include "public/IFileSystemProvider.h"
#include "ICryptoProvider.h"
#include "NonCopyable.h"

namespace lcp
{
#if !DISABLE_NET_PROVIDER
    class INetProvider;
#endif //!DISABLE_NET_PROVIDER

#if !DISABLE_CRL
    class CrlUpdater;
    class ThreadTimer;
#endif //!DISABLE_CRL

    class EncryptionProfilesManager;
    class ICertificate;

#if !DISABLE_CRL
class ICertificateRevocationList;
#endif //!DISABLE_CRL

    class CryptoppCryptoProvider : public ICryptoProvider, public NonCopyable
    {
    public:
        CryptoppCryptoProvider(
            EncryptionProfilesManager * encryptionProfilesManager
#if !DISABLE_NET_PROVIDER
        , INetProvider * netProvider
#endif //!DISABLE_NET_PROVIDER

                , IFileSystemProvider * fileSystemProvider


#if !DISABLE_CRL
        , const std::string & defaultCrlUrl
#endif //!DISABLE_CRL
            );
        ~CryptoppCryptoProvider();

        virtual Status VerifyLicense(
            const std::string & rootCertificateBase64,
            ILicense * license
            );

        virtual Status DecryptUserKey(
            const std::string & userPassphrase,
            ILicense * license,
            KeyType & userKey
            );

        virtual Status DecryptContentKey(
            const KeyType & userKey,
            ILicense * license,
            KeyType & contentKey
            );

        virtual Status CalculateFileHash(
            IReadableStream * readableStream,
            std::vector<unsigned char> & rawHash
            );

        virtual Status ConvertRawToHex(
            const std::vector<unsigned char> & data,
            std::string & hex
            );

        virtual Status ConvertHexToRaw(
            const std::string & hex,
            std::vector<unsigned char> & data
            );

        virtual Status GenerateUuid(std::string & uuid);

        virtual Status DecryptLicenseData(
            const std::string & dataBase64,
            ILicense * license,
            IKeyProvider * keyProvider,
            std::string & decrypted
            );

        virtual Status DecryptPublicationData(
            ILicense * license,
            IKeyProvider * keyProvider,
            const unsigned char * data,
            const size_t dataLength,
            unsigned char * decryptedData,
            size_t * decryptedDataLength,
            const std::string & algorithm
            );

        virtual Status CreateEncryptedPublicationStream(
            ILicense * license,
            IKeyProvider * keyProvider,
            IReadableStream * stream,
            IEncryptedStream ** encStream,
            const std::string & algorithm
            );

#if !DISABLE_CRL
    public:
        Status CheckRevokation(ILicense* license);

    private:
        Status CheckRevokation(ICertificate * providerCertificate);
        Status ProcessRevokation(ICertificate * rootCertificate, ICertificate * providerCertificate);


        std::unique_ptr<ICertificateRevocationList> m_revocationList;

#if !DISABLE_CRL_BACKGROUND_POLL
        std::unique_ptr<ThreadTimer> m_threadTimer;
#endif //!DISABLE_CRL_BACKGROUND_POLL

        IFileSystemProvider * m_fileSystemProvider;

        std::unique_ptr<CrlUpdater> m_crlUpdater;
        std::mutex m_processRevocationSync;
#endif //!DISABLE_CRL

        EncryptionProfilesManager * m_encryptionProfilesManager;
    };
}

#endif //__CRYPTOPP_CRYPTO_PROVIDER_H__
