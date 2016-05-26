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
#include "CertificateRevocationList.h"
#include "CrlUpdater.h"
#include "ThreadTimer.h"
#include "DateTime.h"
#include "LcpUtils.h"
#include "IKeyProvider.h"
#include "CryptoppUtils.h"
#include "Sha256HashAlgorithm.h"
#include "SymmetricAlgorithmEncryptedStream.h"

namespace lcp
{
    CryptoppCryptoProvider::CryptoppCryptoProvider(
        EncryptionProfilesManager * encryptionProfilesManager,
        INetProvider * netProvider,
        const std::string & defaultCrlUrl
        )
        : m_encryptionProfilesManager(encryptionProfilesManager)
    {
        m_revocationList.reset(new CertificateRevocationList());
        m_threadTimer.reset(new ThreadTimer());
        m_crlUpdater.reset(new CrlUpdater(netProvider, m_revocationList.get(), m_threadTimer.get(), defaultCrlUrl));

        m_threadTimer->SetHandler(std::bind(&CrlUpdater::Update, m_crlUpdater.get()));
        m_threadTimer->SetAutoReset(false);

        if (m_crlUpdater->ContainsAnyUrl())
        {
            m_threadTimer->SetUsage(ThreadTimer::DurationUsage);
            m_threadTimer->SetDuration(ThreadTimer::DurationType(ThreadTimer::DurationType::zero()));
            m_threadTimer->Start();
        }
    }

    CryptoppCryptoProvider::~CryptoppCryptoProvider()
    {
        try
        {
            m_crlUpdater->Cancel();
            m_threadTimer->Stop();
        }
        catch (...)
        {
        }
    }

    Status CryptoppCryptoProvider::VerifyLicense(
        const std::string & rootCertificateBase64,
        ILicense * license
        )
    {
        try
        {
            IEncryptionProfile * profile = m_encryptionProfilesManager->GetProfile(license->Crypto()->EncryptionProfile());
            if (profile == nullptr)
            {
                return Status(StatusCode::ErrorCommonEncryptionProfileNotFound);
            }

            if (rootCertificateBase64.empty())
            {
                return Status(StatusCode::ErrorOpeningNoRootCertificate);
            }

            std::unique_ptr<Certificate> rootCertificate;
            try
            {
                rootCertificate.reset(new Certificate(rootCertificateBase64, profile));
            }
            catch (CryptoPP::BERDecodeErr & ex)
            {
                return Status(StatusCode::ErrorOpeningRootCertificateNotValid, ex.GetWhat());
            }

            std::unique_ptr<Certificate> providerCertificate;
            try
            {
                providerCertificate.reset(new Certificate(license->Crypto()->SignatureCertificate(), profile));
            }
            catch (CryptoPP::BERDecodeErr & ex)
            {
                return Status(StatusCode::ErrorOpeningContentProviderCertificateNotValid, ex.GetWhat());
            }

            if (!providerCertificate->VerifyCertificate(rootCertificate.get()))
            {
                return Status(StatusCode::ErrorOpeningContentProviderCertificateNotVerified);
            }

            Status res = this->ProcessRevokation(rootCertificate.get(), providerCertificate.get());
            if (!Status::IsSuccess(res))
            {
                return res;
            }

            if (!providerCertificate->VerifyMessage(license->CanonicalContent(), license->Crypto()->Signature()))
            {
                return Status(StatusCode::ErrorOpeningLicenseSignatureNotValid);
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
                return Status(StatusCode::ErrorOpeningContentProviderCertificateNotStarted);
            }
            else if (lastUpdated > notAfter)
            {
                return Status(StatusCode::ErrorOpeningContentProviderCertificateExpired);
            }
            return Status(StatusCode::ErrorCommonSuccess);
        }
        catch (const CryptoPP::Exception & ex)
        {
            return Status(StatusCode::ErrorOpeningContentProviderCertificateNotVerified, ex.GetWhat());
        }
    }

    Status CryptoppCryptoProvider::DecryptUserKey(
        const std::string & userPassphrase,
        ILicense * license,
        KeyType & userKey
        )
    {
        try
        {
            IEncryptionProfile * profile = m_encryptionProfilesManager->GetProfile(license->Crypto()->EncryptionProfile());
            if (profile == nullptr)
            {
                return Status(StatusCode::ErrorCommonEncryptionProfileNotFound);
            }

            std::unique_ptr<IHashAlgorithm> hashAlgorithm(profile->CreateUserKeyAlgorithm());
            hashAlgorithm->UpdateHash(userPassphrase);
            userKey = hashAlgorithm->Hash();

            std::unique_ptr<ISymmetricAlgorithm> contentKeyAlgorithm(profile->CreateContentKeyAlgorithm(userKey));
            std::string id = contentKeyAlgorithm->Decrypt(license->Crypto()->UserKeyCheck());
            if (!EqualsUtf8(id, license->Id()))
            {
                return Status(StatusCode::ErrorDecryptionUserPassphraseNotValid);
            }
            return Status(StatusCode::ErrorCommonSuccess);
        }
        catch (const CryptoPP::Exception & ex)
        {
            return Status(StatusCode::ErrorDecryptionUserPassphraseNotValid, ex.GetWhat());
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
            IEncryptionProfile * profile = m_encryptionProfilesManager->GetProfile(license->Crypto()->EncryptionProfile());
            if (profile == nullptr)
            {
                return Status(StatusCode::ErrorCommonEncryptionProfileNotFound);
            }

            std::unique_ptr<ISymmetricAlgorithm> contentKeyAlgorithm(profile->CreateContentKeyAlgorithm(userKey));
            std::string decryptedContentKey = contentKeyAlgorithm->Decrypt(license->Crypto()->ContentKey());

            contentKey.assign(decryptedContentKey.begin(), decryptedContentKey.end());
            return Status(StatusCode::ErrorCommonSuccess);
        }
        catch (const CryptoPP::Exception & ex)
        {
            return Status(StatusCode::ErrorDecryptionLicenseEncrypted, ex.GetWhat());
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
            return Status(StatusCode::ErrorDecryptionCommonError, ex.GetWhat());
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
            return Status(StatusCode::ErrorDecryptionCommonError, ex.GetWhat());
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
            return Status(StatusCode::ErrorDecryptionCommonError, ex.GetWhat());
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
            return Status(StatusCode::ErrorDecryptionCommonError, ex.GetWhat());
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
            IEncryptionProfile * profile = m_encryptionProfilesManager->GetProfile(license->Crypto()->EncryptionProfile());
            if (profile == nullptr)
            {
                return Status(StatusCode::ErrorCommonEncryptionProfileNotFound);
            }

            std::unique_ptr<ISymmetricAlgorithm> contentKeyAlgorithm(profile->CreateContentKeyAlgorithm(keyProvider->UserKey()));
            decrypted = contentKeyAlgorithm->Decrypt(dataBase64);
            return Status(StatusCode::ErrorCommonSuccess);
        }
        catch (const CryptoPP::Exception & ex)
        {
            return Status(StatusCode::ErrorDecryptionLicenseEncrypted, ex.GetWhat());
        }
    }

    Status CryptoppCryptoProvider::DecryptPublicationData(
        ILicense * license,
        IKeyProvider * keyProvider,
        const unsigned char * data,
        const size_t dataLength,
        unsigned char * decryptedData,
        size_t * decryptedDataLength
        )
    {
        try
        {
            IEncryptionProfile * profile = m_encryptionProfilesManager->GetProfile(license->Crypto()->EncryptionProfile());
            if (profile == nullptr)
            {
                return Status(StatusCode::ErrorCommonEncryptionProfileNotFound);
            }

            std::unique_ptr<ISymmetricAlgorithm> algorithm(profile->CreatePublicationAlgorithm(keyProvider->ContentKey()));
            *decryptedDataLength = algorithm->Decrypt(
                data, dataLength, decryptedData, *decryptedDataLength
                );

            return Status(StatusCode::ErrorCommonSuccess);
        }
        catch (const CryptoPP::Exception & ex)
        {
            return Status(StatusCode::ErrorDecryptionPublicationEncrypted, ex.GetWhat());
        }
    }

    Status CryptoppCryptoProvider::CreateEncryptedPublicationStream(
        ILicense * license,
        IKeyProvider * keyProvider,
        IReadableStream * stream,
        IEncryptedStream ** encStream
        )
    {
        try
        {
            IEncryptionProfile * profile = m_encryptionProfilesManager->GetProfile(license->Crypto()->EncryptionProfile());
            if (profile == nullptr)
            {
                return Status(StatusCode::ErrorCommonEncryptionProfileNotFound);
            }

            Status res(StatusCode::ErrorCommonSuccess);
            std::unique_ptr<ISymmetricAlgorithm> algorithm(profile->CreatePublicationAlgorithm(keyProvider->ContentKey()));
            *encStream = new SymmetricAlgorithmEncryptedStream(stream, std::move(algorithm));
            return res;
        }
        catch (const CryptoPP::Exception & ex)
        {
            return Status(StatusCode::ErrorDecryptionPublicationEncrypted, ex.GetWhat());
        }
    }

    Status CryptoppCryptoProvider::ProcessRevokation(ICertificate * rootCertificate, ICertificate * providerCertificate)
    {
        m_crlUpdater->UpdateCrlUrls(rootCertificate->DistributionPoints());
        m_crlUpdater->UpdateCrlUrls(providerCertificate->DistributionPoints());

        // First time processing of the CRL
        std::unique_lock<std::mutex> locker(m_processRevocationSync);
        if (m_crlUpdater->ContainsAnyUrl() && !m_revocationList->HasThisUpdateDate())
        {
            if (m_threadTimer->IsRunning())
            {
                m_threadTimer->Stop();
            }

            // Check once more, the CRL state could've been changed during the stop process
            if (!m_revocationList->HasThisUpdateDate())
            {
                // If CRL is absent, update it right before certificate verification
                m_crlUpdater->Update();
            }

            // Start timer which will check CRL for updates periodically or by time point
            m_threadTimer->SetAutoReset(true);
            m_threadTimer->SetUsage(ThreadTimer::DurationUsage);
            m_threadTimer->SetDuration(ThreadTimer::DurationType(CrlUpdater::TenMinutesPeriod));
            m_threadTimer->Start();
        }
        locker.unlock();

        // If exception occurred in the timer thread, re-throw it
        m_threadTimer->RethrowExceptionIfAny();

        if (m_revocationList->SerialNumberRevoked(providerCertificate->SerialNumber()))
        {
            return Status(StatusCode::ErrorOpeningContentProviderCertificateRevoked);
        }
        return Status(StatusCode::ErrorCommonSuccess);
    }
}