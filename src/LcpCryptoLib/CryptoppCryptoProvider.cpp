#include "CryptoppCryptoProvider.h"
#include "CryptoAlgorithmInterfaces.h"
#include "EncryptionProfilesManager.h"
#include "Public/ILicense.h"
#include "Public/ICrypto.h"
#include "Public/IFileSystemProvider.h"
#include "Certificate.h"
#include "DateTime.h"
#include "IKeyProvider.h"
#include "CryptoppUtils.h"
#include "Sha256HashAlgorithm.h"
#include "SymmetricAlgorithmEncryptedStream.h"

namespace lcp
{
    CryptoppCryptoProvider::CryptoppCryptoProvider(EncryptionProfilesManager * encryptionProfilesManager)
        : m_encryptionProfilesManager(encryptionProfilesManager)
    {
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
                return Status(StCodeCover::ErrorCommonEncryptionProfileNotFound);
            }

            if (rootCertificateBase64.empty())
            {
                return Status(StCodeCover::ErrorOpeningNoRootCertificate);
            }

            std::unique_ptr<Certificate> rootCertificate;
            try
            {
                rootCertificate.reset(new Certificate(rootCertificateBase64, profile));
            }
            catch (CryptoPP::BERDecodeErr & ex)
            {
                return Status(StCodeCover::ErrorOpeningRootCertificateNotValid, ex.GetWhat());
            }

            std::unique_ptr<Certificate> providerCertificate;
            try
            {
                providerCertificate.reset(new Certificate(license->Crypto()->SignatureCertificate(), profile));
            }
            catch (CryptoPP::BERDecodeErr & ex)
            {
                return Status(StCodeCover::ErrorOpeningContentProviderCertificateNotValid, ex.GetWhat());
            }

            if (!providerCertificate->VerifyCertificate(rootCertificate.get()))
            {
                return Status(StCodeCover::ErrorOpeningContentProviderCertificateNotVerified);
            }
            if (!providerCertificate->VerifyMessage(license->Content(), license->Crypto()->Signature()))
            {
                return Status(StCodeCover::ErrorOpeningLicenseSignatureNotValid);
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
                return Status(StCodeCover::ErrorOpeningContentProviderCertificateNotStarted);
            }
            else if (lastUpdated > notAfter)
            {
                return Status(StCodeCover::ErrorOpeningContentProviderCertificateExpired);
            }
            return Status(StCodeCover::ErrorCommonSuccess);
        }
        catch (const CryptoPP::Exception & ex)
        {
            return Status(StCodeCover::ErrorOpeningContentProviderCertificateNotVerified, ex.GetWhat());
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
                return Status(StCodeCover::ErrorCommonEncryptionProfileNotFound);
            }

            std::unique_ptr<IHashAlgorithm> hashAlgorithm(profile->CreateUserKeyAlgorithm());
            hashAlgorithm->UpdateHash(userPassphrase);
            userKey = hashAlgorithm->Hash();

            std::unique_ptr<ISymmetricAlgorithm> contentKeyAlgorithm(profile->CreateContentKeyAlgorithm(userKey));
            std::string id = contentKeyAlgorithm->Decrypt(license->Crypto()->UserKeyCheck());
            if (!EqualsUtf8(id, license->Id()))
            {
                return Status(StCodeCover::ErrorDecryptionUserPassphraseNotValid);
            }
            return Status(StCodeCover::ErrorCommonSuccess);
        }
        catch (const CryptoPP::Exception & ex)
        {
            return Status(StCodeCover::ErrorDecryptionUserPassphraseNotValid, ex.GetWhat());
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
                return Status(StCodeCover::ErrorCommonEncryptionProfileNotFound);
            }

            std::unique_ptr<ISymmetricAlgorithm> contentKeyAlgorithm(profile->CreateContentKeyAlgorithm(userKey));
            std::string decryptedContentKey = contentKeyAlgorithm->Decrypt(license->Crypto()->ContentKey());

            contentKey.assign(decryptedContentKey.begin(), decryptedContentKey.end());
            return Status(StCodeCover::ErrorCommonSuccess);
        }
        catch (const CryptoPP::Exception & ex)
        {
            return Status(StCodeCover::ErrorDecryptionLicenseEncrypted, ex.GetWhat());
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
            size_t fileSize = readableStream->Size();
            while (read != fileSize)
            {
                sizeToRead = (fileSize - read > bufferSize) ? bufferSize : fileSize - read;
                readableStream->Read(buffer.data(), sizeToRead);
                algorithm.UpdateHash(buffer.data(), sizeToRead);
                read += sizeToRead;
            }
            rawHash = algorithm.Hash();

            return Status(StCodeCover::ErrorCommonSuccess);
        }
        catch (const CryptoPP::Exception & ex)
        {
            return Status(StCodeCover::ErrorDecryptionCommonError, ex.GetWhat());
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
            return Status(StCodeCover::ErrorCommonSuccess);
        }
        catch (const CryptoPP::Exception & ex)
        {
            return Status(StCodeCover::ErrorDecryptionCommonError, ex.GetWhat());
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
            return Status(StCodeCover::ErrorCommonSuccess);
        }
        catch (const CryptoPP::Exception & ex)
        {
            return Status(StCodeCover::ErrorDecryptionCommonError, ex.GetWhat());
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
                return Status(StCodeCover::ErrorCommonEncryptionProfileNotFound);
            }

            std::unique_ptr<ISymmetricAlgorithm> contentKeyAlgorithm(profile->CreateContentKeyAlgorithm(keyProvider->UserKey()));
            decrypted = contentKeyAlgorithm->Decrypt(dataBase64);
            return Status(StCodeCover::ErrorCommonSuccess);
        }
        catch (const CryptoPP::Exception & ex)
        {
            return Status(StCodeCover::ErrorDecryptionLicenseEncrypted, ex.GetWhat());
        }
    }

    Status CryptoppCryptoProvider::DecryptPublicationData(
        ILicense * license,
        IKeyProvider * keyProvider,
        const unsigned char * data,
        const size_t dataLength,
        unsigned char * decryptedData,
        size_t inDecryptedDataLength,
        size_t * outDecryptedDataLength
        )
    {
        try
        {
            IEncryptionProfile * profile = m_encryptionProfilesManager->GetProfile(license->Crypto()->EncryptionProfile());
            if (profile == nullptr)
            {
                return Status(StCodeCover::ErrorCommonEncryptionProfileNotFound);
            }

            std::unique_ptr<ISymmetricAlgorithm> algorithm(profile->CreatePublicationAlgorithm(keyProvider->ContentKey()));
            *outDecryptedDataLength = algorithm->Decrypt(
                data, dataLength, decryptedData, inDecryptedDataLength
                );

            return Status(StCodeCover::ErrorCommonSuccess);
        }
        catch (const CryptoPP::Exception & ex)
        {
            return Status(StCodeCover::ErrorDecryptionPublicationEncrypted, ex.GetWhat());
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
                return Status(StCodeCover::ErrorCommonEncryptionProfileNotFound);
            }

            Status res(StCodeCover::ErrorCommonSuccess);
            std::unique_ptr<ISymmetricAlgorithm> algorithm(profile->CreatePublicationAlgorithm(keyProvider->ContentKey()));
            *encStream = new SymmetricAlgorithmEncryptedStream(stream, std::move(algorithm));
            return res;
        }
        catch (const CryptoPP::Exception & ex)
        {
            return Status(StCodeCover::ErrorDecryptionPublicationEncrypted, ex.GetWhat());
        }
    }
}