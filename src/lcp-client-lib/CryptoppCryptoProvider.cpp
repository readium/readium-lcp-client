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

#include <functional>
#include <chrono>
#include "CryptoppCryptoProvider.h"
#include "CryptoAlgorithmInterfaces.h"
#include "EncryptionProfilesManager.h"
#include "public/ILicense.h"
#include "public/ICrypto.h"
#include "Certificate.h"

#if !DISABLE_CRL
#include "CertificateRevocationList.h"
#include "CrlUpdater.h"
#include "ThreadTimer.h"
#endif //!DISABLE_CRL

#include "DateTime.h"
#include "LcpUtils.h"
#include "IKeyProvider.h"
#include "CryptoppUtils.h"
#include "Sha256HashAlgorithm.h"
#include "SymmetricAlgorithmEncryptedStream.h"

namespace lcp
{
    CryptoppCryptoProvider::CryptoppCryptoProvider(
        EncryptionProfilesManager * encryptionProfilesManager

#if !DISABLE_NET_PROVIDER
    , INetProvider * netProvider
#endif //!DISABLE_NET_PROVIDER

            , IFileSystemProvider * fileSystemProvider

#if !DISABLE_CRL
        , const std::string & defaultCrlUrl
#endif //!DISABLE_CRL
        )
        :
            m_encryptionProfilesManager(encryptionProfilesManager)

            , m_fileSystemProvider(fileSystemProvider)

    {
#if !DISABLE_CRL
        m_revocationList.reset(new CertificateRevocationList());

#if !DISABLE_CRL_BACKGROUND_POLL
        m_threadTimer.reset(new ThreadTimer());
#endif //!DISABLE_CRL_BACKGROUND_POLL

        m_crlUpdater.reset(new CrlUpdater(
#if !DISABLE_NET_PROVIDER
                netProvider,
#endif //!DISABLE_NET_PROVIDER

                m_fileSystemProvider,

                m_revocationList.get(),

#if !DISABLE_CRL_BACKGROUND_POLL
                m_threadTimer.get(),
#endif //!DISABLE_CRL_BACKGROUND_POLL

                defaultCrlUrl));

#if !DISABLE_CRL_BACKGROUND_POLL
        m_threadTimer->SetHandler(std::bind(&CrlUpdater::Update, m_crlUpdater.get()));
        m_threadTimer->SetAutoReset(false);

        if (m_crlUpdater->ContainsAnyUrl())
        {
            m_threadTimer->SetUsage(ThreadTimer::DurationUsage);
            m_threadTimer->SetDuration(ThreadTimer::DurationType(ThreadTimer::DurationType::zero()));
            m_threadTimer->Start();
        }
#endif //!DISABLE_CRL_BACKGROUND_POLL

#endif //!DISABLE_CRL
    }

    CryptoppCryptoProvider::~CryptoppCryptoProvider()
    {
#if !DISABLE_CRL
        try
        {
            m_crlUpdater->Cancel();

#if !DISABLE_CRL_BACKGROUND_POLL
            m_threadTimer->Stop();
#endif //!DISABLE_CRL_BACKGROUND_POLL
        }
        catch (...)
        {
        }
#endif //!DISABLE_CRL
    }

    Status CryptoppCryptoProvider::VerifyLicense(
        const std::string & rootCertificateBase64,
        ILicense * license
        )
    {
        try
        {
#if ENABLE_PROFILE_NAMES
            IEncryptionProfile * profile = m_encryptionProfilesManager->GetProfile(license->Crypto()->EncryptionProfile());
            if (profile == nullptr)
            {
                return Status(StatusCode::ErrorCommonEncryptionProfileNotFound, "ErrorCommonEncryptionProfileNotFound");
            }
#else
            IEncryptionProfile * profile = m_encryptionProfilesManager->GetProfile();
#endif //ENABLE_PROFILE_NAMES

            if (rootCertificateBase64.empty())
            {
                return Status(StatusCode::ErrorOpeningNoRootCertificate, "ErrorOpeningNoRootCertificate");
            }

            std::unique_ptr<Certificate> rootCertificate;
            try
            {
                rootCertificate.reset(new Certificate(rootCertificateBase64, profile));
            }
            catch (CryptoPP::BERDecodeErr & ex)
            {
                return Status(StatusCode::ErrorOpeningRootCertificateNotValid, "ErrorOpeningRootCertificateNotValid: " + ex.GetWhat());
            }

            std::unique_ptr<Certificate> providerCertificate;
            try
            {
                providerCertificate.reset(new Certificate(license->Crypto()->SignatureCertificate(), profile));
            }
            catch (CryptoPP::BERDecodeErr & ex)
            {
                return Status(StatusCode::ErrorOpeningContentProviderCertificateNotValid, "ErrorOpeningContentProviderCertificateNotValid: " + ex.GetWhat());
            }

            if (!providerCertificate->VerifyCertificate(rootCertificate.get()))
            {
                return Status(StatusCode::ErrorOpeningContentProviderCertificateNotVerified, "ErrorOpeningContentProviderCertificateNotVerified");
            }

#if !DISABLE_CRL
            Status res = this->ProcessRevokation(rootCertificate.get(), providerCertificate.get());
            if (!Status::IsSuccess(res))
            {
                return res;
            }
#endif //!DISABLE_CRL

            //providerCertificate->VerifyMessage
            lcp::ISignatureAlgorithm* signatureAlgorithm = profile->CreateSignatureAlgorithm(providerCertificate->PublicKey(), license->Crypto()->SignatureAlgorithm());
            if (!signatureAlgorithm->VerifySignature(license->CanonicalContent(), license->Crypto()->Signature()))
            {
                return Status(StatusCode::ErrorOpeningLicenseSignatureNotValid, "ErrorOpeningLicenseSignatureNotValid");
            }

            DateTime notBefore(providerCertificate->NotBeforeDate());
            DateTime notAfter(providerCertificate->NotAfterDate());

            DateTime lastUpdated;
            if (!license->Updated().empty())
            {
                lastUpdated = DateTime(license->Updated());
            }
            else
            {
                lastUpdated = DateTime(license->Issued());
            }

            if (lastUpdated < notBefore)
            {
                return Status(StatusCode::ErrorOpeningContentProviderCertificateNotStarted, "ErrorOpeningContentProviderCertificateNotStarted");
            }
            else if (lastUpdated > notAfter)
            {
                return Status(StatusCode::ErrorOpeningContentProviderCertificateExpired, "ErrorOpeningContentProviderCertificateExpired");
            }
            return Status(StatusCode::ErrorCommonSuccess);
        }
        catch (const CryptoPP::Exception & ex)
        {
            return Status(StatusCode::ErrorOpeningContentProviderCertificateNotVerified, "ErrorOpeningContentProviderCertificateNotVerified: " + ex.GetWhat());
        }
    }

    Status CryptoppCryptoProvider::LegacyPassphraseUserKey(
            const KeyType & userKey1,
            KeyType & userKey2
    )
    {
        try
        {
            userKey2.assign(userKey1.begin(), userKey1.end());

            return Status(StatusCode::ErrorCommonSuccess);
        }
        catch (const std::exception & ex)
        {
            return Status(StatusCode::ErrorDecryptionUserPassphraseNotValid, "ErrorDecryptionUserPassphraseNotValid: " + std::string(ex.what()));
        }
    }

    Status CryptoppCryptoProvider::DecryptUserKey(
        const std::string & userPassphrase,
        ILicense * license,
        KeyType & userKey1,
        KeyType & userKey2
        )
    {
        try
        {
#if ENABLE_PROFILE_NAMES
            IEncryptionProfile * profile = m_encryptionProfilesManager->GetProfile(license->Crypto()->EncryptionProfile());
            if (profile == nullptr)
            {
                return Status(StatusCode::ErrorCommonEncryptionProfileNotFound, "ErrorCommonEncryptionProfileNotFound");
            }
#else
            IEncryptionProfile * profile = m_encryptionProfilesManager->GetProfile();
#endif //ENABLE_PROFILE_NAMES

            std::unique_ptr<IHashAlgorithm> hashAlgorithm(profile->CreateUserKeyAlgorithm());
            hashAlgorithm->UpdateHash(userPassphrase);
            userKey1 = hashAlgorithm->Hash();

            Status resx = this->LegacyPassphraseUserKey(userKey1, userKey2);
            if (!Status::IsSuccess(resx)) {
                return resx;
            }

            //http://www.w3.org/2009/xmlenc11#aes256-gcm
            //http://www.w3.org/2001/04/xmlenc#aes256-cbc
            const std::string algorithm = license->Crypto()->ContentKeyAlgorithm();

            std::unique_ptr<ISymmetricAlgorithm> contentKeyAlgorithm(profile->CreateContentKeyAlgorithm(userKey2, algorithm));
            std::string id = contentKeyAlgorithm->Decrypt(license->Crypto()->UserKeyCheck());
            try
            {
                if (!EqualsUtf8(id, license->Id()))
                {
                    return Status(StatusCode::ErrorDecryptionUserPassphraseNotValid, "ErrorDecryptionUserPassphraseNotValid");
                }
            }
            catch (const std::exception & exc) // utf8::invalid_utf8
            {
                return Status(StatusCode::ErrorDecryptionUserPassphraseNotValid, "ErrorDecryptionUserPassphraseNotValid: " + std::string(exc.what()));
            }
            return Status(StatusCode::ErrorCommonSuccess);
        }
        catch (const CryptoPP::Exception & ex)
        {
            return Status(StatusCode::ErrorDecryptionUserPassphraseNotValid, "ErrorDecryptionUserPassphraseNotValid: " + ex.GetWhat());
        }
    }

    Status CryptoppCryptoProvider::DecryptContentKey(
        const KeyType & userKey,
        ILicense * license,
        KeyType & contentKey
        )
    {
        try
        {
#if ENABLE_PROFILE_NAMES
            IEncryptionProfile * profile = m_encryptionProfilesManager->GetProfile(license->Crypto()->EncryptionProfile());
            if (profile == nullptr)
            {
                return Status(StatusCode::ErrorCommonEncryptionProfileNotFound, "ErrorCommonEncryptionProfileNotFound");
            }
#else
            IEncryptionProfile * profile = m_encryptionProfilesManager->GetProfile();
#endif //ENABLE_PROFILE_NAMES

            //http://www.w3.org/2009/xmlenc11#aes256-gcm
            //http://www.w3.org/2001/04/xmlenc#aes256-cbc
            const std::string algorithm_ = license->Crypto()->ContentKeyAlgorithm();
            std::unique_ptr<ISymmetricAlgorithm> contentKeyAlgorithm_(profile->CreateContentKeyAlgorithm(userKey, algorithm_));
            std::string id = contentKeyAlgorithm_->Decrypt(license->Crypto()->UserKeyCheck());
            try
            {
                if (!EqualsUtf8(id, license->Id()))
                {
                    return Status(StatusCode::ErrorDecryptionUserPassphraseNotValid, "ErrorDecryptionUserPassphraseNotValid");
                }
            }
            catch (const std::exception & exc) // utf8::invalid_utf8
            {
                return Status(StatusCode::ErrorDecryptionUserPassphraseNotValid, "ErrorDecryptionUserPassphraseNotValid: " + std::string(exc.what()));
            }

            const std::string algorithm = license->Crypto()->ContentKeyAlgorithm();
            std::unique_ptr<ISymmetricAlgorithm> contentKeyAlgorithm(profile->CreateContentKeyAlgorithm(userKey, algorithm));
            std::string decryptedContentKey = contentKeyAlgorithm->Decrypt(license->Crypto()->ContentKey());

            contentKey.assign(decryptedContentKey.begin(), decryptedContentKey.end());
            return Status(StatusCode::ErrorCommonSuccess);
        }
        catch (const CryptoPP::Exception & ex)
        {
            return Status(StatusCode::ErrorDecryptionLicenseEncrypted, "ErrorDecryptionLicenseEncrypted: " + ex.GetWhat());
        }
    }

    Status CryptoppCryptoProvider::CalculateFileHash(
        IReadableStream * readableStream,
        std::vector<unsigned char> & rawHash
        )
    {
        try
        {
            Sha256HashAlgorithm algorithm;
            size_t bufferSize = 1024 * 1024;
            std::vector<unsigned char> buffer(bufferSize);
            
            size_t read = 0;
            size_t sizeToRead = bufferSize;
            size_t fileSize = static_cast<size_t>(readableStream->Size());
            while (read != fileSize)
            {
                sizeToRead = (fileSize - read > bufferSize) ? bufferSize : fileSize - read;
                readableStream->Read(buffer.data(), sizeToRead);
                algorithm.UpdateHash(buffer.data(), sizeToRead);
                read += sizeToRead;
            }
            rawHash = algorithm.Hash();

            return Status(StatusCode::ErrorCommonSuccess);
        }
        catch (const CryptoPP::Exception & ex)
        {
            return Status(StatusCode::ErrorDecryptionCommonError, "ErrorDecryptionCommonError: " + ex.GetWhat());
        }
    }

    Status CryptoppCryptoProvider::ConvertRawToHex(
        const std::vector<unsigned char> & data,
        std::string & hex
        )
    {
        try
        {
            hex = CryptoppUtils::RawToHex(data);
            return Status(StatusCode::ErrorCommonSuccess);
        }
        catch (const CryptoPP::Exception & ex)
        {
            return Status(StatusCode::ErrorDecryptionCommonError, "ErrorDecryptionCommonError: " + ex.GetWhat());
        }
    }

    Status CryptoppCryptoProvider::ConvertHexToRaw(
        const std::string & hex,
        std::vector<unsigned char> & data
        )
    {
        try
        {
            data = CryptoppUtils::HexToRaw(hex);
            return Status(StatusCode::ErrorCommonSuccess);
        }
        catch (const CryptoPP::Exception & ex)
        {
            return Status(StatusCode::ErrorDecryptionCommonError, "ErrorDecryptionCommonError: " + ex.GetWhat());
        }
    }

    Status CryptoppCryptoProvider::GenerateUuid(std::string & uuid)
    {
        try
        {
            uuid = CryptoppUtils::GenerateUuid();
            return Status(StatusCode::ErrorCommonSuccess);
        }
        catch (const CryptoPP::Exception & ex)
        {
            return Status(StatusCode::ErrorDecryptionCommonError, "ErrorDecryptionCommonError: " + ex.GetWhat());
        }
    }

    Status CryptoppCryptoProvider::DecryptLicenseData(
        const std::string & dataBase64,
        ILicense * license,
        IKeyProvider * keyProvider,
        std::string & decrypted
        )
    {
        try
        {
#if ENABLE_PROFILE_NAMES
            IEncryptionProfile * profile = m_encryptionProfilesManager->GetProfile(license->Crypto()->EncryptionProfile());
            if (profile == nullptr)
            {
                return Status(StatusCode::ErrorCommonEncryptionProfileNotFound, "ErrorCommonEncryptionProfileNotFound");
            }
#else
            IEncryptionProfile * profile = m_encryptionProfilesManager->GetProfile();
#endif //ENABLE_PROFILE_NAMES

            //http://www.w3.org/2009/xmlenc11#aes256-gcm
            //http://www.w3.org/2001/04/xmlenc#aes256-cbc
            const std::string algorithm = license->Crypto()->ContentKeyAlgorithm();

            std::unique_ptr<ISymmetricAlgorithm> contentKeyAlgorithm(profile->CreateContentKeyAlgorithm(keyProvider->UserKey(), algorithm));
            decrypted = contentKeyAlgorithm->Decrypt(dataBase64);
            return Status(StatusCode::ErrorCommonSuccess);
        }
        catch (const CryptoPP::Exception & ex)
        {
            return Status(StatusCode::ErrorDecryptionLicenseEncrypted, "ErrorDecryptionLicenseEncrypted: " + ex.GetWhat());
        }
    }

    Status CryptoppCryptoProvider::DecryptPublicationData(
        ILicense * license,
        IKeyProvider * keyProvider,
        const unsigned char * data,
        const size_t dataLength,
        unsigned char * decryptedData,
        size_t * decryptedDataLength,
        const std::string & algorithm
        )
    {
        try
        {
#if ENABLE_PROFILE_NAMES
            IEncryptionProfile * profile = m_encryptionProfilesManager->GetProfile(license->Crypto()->EncryptionProfile());
            if (profile == nullptr)
            {
                return Status(StatusCode::ErrorCommonEncryptionProfileNotFound, "ErrorCommonEncryptionProfileNotFound");
            }
#else
            IEncryptionProfile * profile = m_encryptionProfilesManager->GetProfile();
#endif //ENABLE_PROFILE_NAMES

            std::unique_ptr<ISymmetricAlgorithm> algo(profile->CreatePublicationAlgorithm(keyProvider->ContentKey(), algorithm));
            *decryptedDataLength = algo->Decrypt(
                data, dataLength, decryptedData, *decryptedDataLength
                );

            return Status(StatusCode::ErrorCommonSuccess);
        }
        catch (const CryptoPP::Exception & ex)
        {
            return Status(StatusCode::ErrorDecryptionPublicationEncrypted, "ErrorDecryptionPublicationEncrypted: " + ex.GetWhat());
        }
    }

    Status CryptoppCryptoProvider::CreateEncryptedPublicationStream(
        ILicense * license,
        IKeyProvider * keyProvider,
        IReadableStream * stream,
        IEncryptedStream ** encStream,
        const std::string & algorithm
        )
    {
        try
        {
#if ENABLE_PROFILE_NAMES
            IEncryptionProfile * profile = m_encryptionProfilesManager->GetProfile(license->Crypto()->EncryptionProfile());
            if (profile == nullptr)
            {
                return Status(StatusCode::ErrorCommonEncryptionProfileNotFound, "ErrorCommonEncryptionProfileNotFound");
            }
#else
            IEncryptionProfile * profile = m_encryptionProfilesManager->GetProfile();
#endif //ENABLE_PROFILE_NAMES

            Status res(StatusCode::ErrorCommonSuccess);
            std::unique_ptr<ISymmetricAlgorithm> algo(profile->CreatePublicationAlgorithm(keyProvider->ContentKey(), algorithm));
            *encStream = new SymmetricAlgorithmEncryptedStream(stream, std::move(algo));
            return res;
        }
        catch (const CryptoPP::Exception & ex)
        {
            return Status(StatusCode::ErrorDecryptionPublicationEncrypted, "ErrorDecryptionPublicationEncrypted: " + ex.GetWhat());
        }
    }

#if !DISABLE_CRL

    Status CryptoppCryptoProvider::CheckRevokation(ILicense* license) {

#if ENABLE_PROFILE_NAMES
        IEncryptionProfile * profile = m_encryptionProfilesManager->GetProfile(license->Crypto()->EncryptionProfile());
            if (profile == nullptr)
            {
                return Status(StatusCode::ErrorCommonEncryptionProfileNotFound, "ErrorCommonEncryptionProfileNotFound");
            }
#else
        IEncryptionProfile * profile = m_encryptionProfilesManager->GetProfile();
#endif //ENABLE_PROFILE_NAMES

        std::unique_ptr<lcp::Certificate> providerCertificate;
        try {
            providerCertificate.reset(
                    new lcp::Certificate(license->Crypto()->SignatureCertificate(), profile));
        }
        catch (std::exception &ex) {
            return Status(StatusCode::ErrorOpeningContentProviderCertificateNotValid,
                          "ErrorOpeningContentProviderCertificateNotValid: " +
                          std::string(ex.what()));
        }

        return this->CheckRevokation(providerCertificate.get());
    }

    Status CryptoppCryptoProvider::CheckRevokation(ICertificate * providerCertificate) {

        if (m_revocationList->SerialNumberRevoked(providerCertificate->SerialNumber())) {
            return Status(StatusCode::ErrorOpeningContentProviderCertificateRevoked,
                          "ErrorOpeningContentProviderCertificateRevoked");
        }

        return StatusCode::ErrorCommonSuccess;
    }

    Status CryptoppCryptoProvider::ProcessRevokation(ICertificate * rootCertificate, ICertificate * providerCertificate)
    {
        m_crlUpdater->UpdateCrlUrls(rootCertificate->DistributionPoints());
        m_crlUpdater->UpdateCrlUrls(providerCertificate->DistributionPoints());

        // First time processing of the CRL
        std::unique_lock<std::mutex> locker(m_processRevocationSync);
        if (m_crlUpdater->ContainsAnyUrl() && !m_revocationList->HasThisUpdateDate())
        {
#if !DISABLE_CRL_BACKGROUND_POLL
            if (m_threadTimer->IsRunning())
            {
                m_threadTimer->Stop();
            }
#endif //!DISABLE_CRL_BACKGROUND_POLL

//            // Check once more, the CRL state could've been changed during the stop process
//            if (!m_revocationList->HasThisUpdateDate())
//            {
//                // If CRL is absent, update it right before certificate verification
//                m_crlUpdater->Update();
//            }

#if !DISABLE_CRL_BACKGROUND_POLL
            // Start timer which will check CRL for updates periodically or by time point
            m_threadTimer->SetAutoReset(true);
            m_threadTimer->SetUsage(ThreadTimer::DurationUsage);

            //std::function<void()>
            m_threadTimer->SetHandler([&]{
                m_crlUpdater->Update();
            }); // std::bind(&CrlUpdater::Update, &m_crlUpdater);

            m_threadTimer->SetDuration(ThreadTimer::DurationType(CrlUpdater::TenMinutesPeriod));
            m_threadTimer->Start();
#endif //!DISABLE_CRL_BACKGROUND_POLL
        }
        locker.unlock();

#if !DISABLE_CRL_BACKGROUND_POLL
        // If exception occurred in the timer thread, re-throw it
        m_threadTimer->RethrowExceptionIfAny();
#endif //!DISABLE_CRL_BACKGROUND_POLL


        Status resx = this->CheckRevokation(providerCertificate);
        if (!Status::IsSuccess(resx))
        {
            return resx;
        }
//
//        // TODO: only for testing fake mock revocation!!
//        // TODO: REMOVE !
//        m_revocationList->InsertRevokedSerialNumber(providerCertificate->SerialNumber());

        return Status(StatusCode::ErrorCommonSuccess);
    }
#endif //!DISABLE_CRL
}
