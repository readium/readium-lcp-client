//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#include "LcpService.h"
#include "CryptoLcpNode.h"
#include "LinksLcpNode.h"
#include "UserLcpNode.h"
#include "RightsLcpNode.h"
#include "RootLcpNode.h"
#include "LcpUtils.h"
#include "JsonValueReader.h"
#include "JsonCanonicalizer.h"
#include "EncryptionProfilesManager.h"
#include "CryptoppCryptoProvider.h"
#include "SimpleKeyProvider.h"
#include "public/IStorageProvider.h"
#include "Acquisition.h"
#include "RightsService.h"

#if !DISABLE_LSD
#include "IncludeMacros.h"

ZIPLIB_INCLUDE_START
#include "ziplib/Source/ZipLib/ZipFile.h"
ZIPLIB_INCLUDE_END

//#include "DownloadInMemoryRequest.h"
#include "DownloadInFileRequest.h"

#include "DateTime.h"
#include "ThreadTimer.h"

#endif //!DISABLE_LSD

namespace lcp
{
#if !DISABLE_LSD
    class BufferRedableStreamAdapter : public IReadableStream
    {
    public:
        BufferRedableStreamAdapter(const std::vector<unsigned char> &buffer) : m_buffer(buffer), m_pos(0) {}

        virtual void Read(unsigned char * pBuffer, int64_t sizeToRead)
        {
            int64_t available = this->Size() - m_pos;
            if (sizeToRead <= 0) {
                return;
            }
            if (sizeToRead > available) {
                sizeToRead = available;
            }
            //std::memcpy(pBuffer, sizeToRead);
            for (int i = 0; i < sizeToRead; i++) {
                char c = m_buffer[m_pos+i];
                *(pBuffer+i) = c;
            }
        }

        virtual void SetReadPosition(int64_t pos)
        {
            m_pos = pos;
            if (m_pos < 0) {
                m_pos = 0;
            } else if (m_pos >= this->Size()) {
                m_pos = (this->Size()-1);
            }
        }

        virtual int64_t ReadPosition() const
        {
            return m_pos;
        }

        virtual int64_t Size()
        {
            return m_buffer.size();
        }

    protected:
        std::vector<unsigned char> m_buffer;
        int64_t m_pos;
    };
#endif //!DISABLE_LSD

#if !DISABLE_LSD
    static std::string const LcpLicensePath = "META-INF/license.lcpl";

    /*static*/ std::string LcpService::StatusType = "application/vnd.readium.license.status.v1.0+json";
    // https://github.com/drminside/lsd_client/blob/master/json_schema_lsd.json
#endif //!DISABLE_LSD

    /*static*/ std::string LcpService::UnknownProvider = "UnknownProvider";
    /*static*/ std::string LcpService::UnknownUserId = "UnknownUserId";

    LcpService::LcpService(
        const std::string & rootCertificate,
        INetProvider * netProvider,
        IStorageProvider * storageProvider,
        IFileSystemProvider * fileSystemProvider,
        const std::string & defaultCrlUrl
        )
        : m_rootCertificate(rootCertificate)
        , m_netProvider(netProvider)
        , m_storageProvider(storageProvider)
        , m_fileSystemProvider(fileSystemProvider)
        , m_rightsService(new RightsService(m_storageProvider, UnknownUserId))
        , m_jsonReader(new JsonValueReader())
        , m_encryptionProfilesManager(new EncryptionProfilesManager())
        , m_cryptoProvider(new CryptoppCryptoProvider(m_encryptionProfilesManager.get(), m_netProvider, defaultCrlUrl))
#if !DISABLE_LSD
        , m_lsdRequestStatus(Status(lcp::StatusCode::ErrorCommonSuccess))
#endif //!DISABLE_LSD
    {
    }


    Status LcpService::OpenLicense(
#if !DISABLE_LSD
            const ePub3::string & publicationPath,
#endif //!DISABLE_LSD
            const std::string & licenseJson, std::promise<ILicense*> & licensePromise)
    {
#if !DISABLE_LSD
        m_publicationPath = publicationPath;

        bool ignoreStatusDocument = m_publicationPath.empty();
#endif //!DISABLE_LSD

        try
        {
            ILicense* license_;
            ILicense** license = &license_;
            
            if (license == nullptr)
            {
                licensePromise.set_value(nullptr);
                throw std::invalid_argument("license is nullptr");
            }

            std::string canonicalJson = this->CalculateCanonicalForm(licenseJson);

            bool foundLicense = this->FindLicense(canonicalJson, license);
            if (foundLicense) {
                Status res = Status(StatusCode::ErrorCommonSuccess);

                if (!(*license)->Decrypted()) {
                    // THIS SHOULD NEVER HAPPEN!
                    res = this->CheckDecrypted(*license);
                }

#if !DISABLE_LSD
                if (ignoreStatusDocument) {
#endif //!DISABLE_LSD
                    licensePromise.set_value(*license);
                    return res;
#if !DISABLE_LSD
                }
#endif //!DISABLE_LSD
            }

            std::unique_ptr<CryptoLcpNode> cryptoNode(new CryptoLcpNode(m_encryptionProfilesManager.get()));
            std::unique_ptr<LinksLcpNode> linksNode(new LinksLcpNode());
            std::unique_ptr<UserLcpNode> userNode(new UserLcpNode());
            std::unique_ptr<RightsLcpNode> rightsNode(new RightsLcpNode());
            std::unique_ptr<RootLcpNode> rootNode(new RootLcpNode(
                licenseJson,
                canonicalJson,
                cryptoNode.get(),
                linksNode.get(),
                userNode.get(),
                rightsNode.get())
                );

            rootNode->AddChildNode(std::move(cryptoNode));
            rootNode->AddChildNode(std::move(linksNode));
            rootNode->AddChildNode(std::move(userNode));
            rootNode->AddChildNode(std::move(rightsNode));

            auto parentValue = rapidjson::Value(rapidjson::kNullType);
            rootNode->ParseNode(parentValue, m_jsonReader.get());

            Status res = rootNode->VerifyNode(rootNode.get(), this, m_cryptoProvider.get());
            if (!Status::IsSuccess(res)) {
                licensePromise.set_value(nullptr);
                return res;
            }

            if (!foundLicense) { // we check because foundLicense can be true when !DISABLE_LSD && !ignoreStatusDocument
                std::unique_lock<std::mutex> locker(m_licensesSync);
                auto insertRes = m_licenses.insert(
                        std::make_pair(std::move(canonicalJson), std::move(rootNode)));
                if (!insertRes.second) {
                    licensePromise.set_value(nullptr);
                    return Status(StatusCode::ErrorOpeningDuplicateLicenseInstance,
                                  "Two License instances with the same canonical form");
                }
                *license = insertRes.first->second.get();
                locker.unlock();
            }

            Status result = Status(StatusCode::ErrorCommonSuccess);

            result = this->CheckDecrypted(license_);

#if !DISABLE_LSD
            if (!ignoreStatusDocument) {
                result = this->ProcessLicenseStatusDocument(license, licensePromise);
            }

            if (result.Code == StatusCode::ErrorStatusDocumentNewLicense) { // SUCCESS
                // result = Status(StatusCode::ErrorCommonSuccess);

                // Execution flow: on the client side of the LcpService::OpenLicense() API (i.e. LCP Content Module), the call to licensePromise.get() is blocking,
                // while the LSD downloader asynchronously acquires the updated license.
            } else {
#endif //!DISABLE_LSD

                licensePromise.set_value(license_);
#if !DISABLE_LSD
            }
#endif //!DISABLE_LSD

            return result;
        }
        catch (const StatusException & ex)
        {
            licensePromise.set_value(nullptr);
            return ex.ResultStatus();
        }
    }

    Status LcpService::CheckDecrypted(ILicense* license) {

        Status result = Status(StatusCode::ErrorCommonSuccess);

        if (!license->Decrypted())
        {
            result = this->DecryptLicenseOnOpening(license);
            
            if (result.Code == StatusCode::ErrorDecryptionLicenseEncrypted) {
                //ASSERT (!license->Decrypted())

                // failure to decrypt using stored user key, we need user passphrase input
                result = Status(StatusCode::ErrorCommonSuccess);
                    
                // Execution flow: on the client side of the LcpService::OpenLicense() API (i.e. LCP Content Module), LICENSE->Decrypted() boolean is checked, and if false then ePub3::ContentModuleExceptionDecryptFlow is raised, then captured quietly where ePub3::Container::OpenContainer() is invoked. Meanwhile, the LCP Crendential Handler triggers asynchronous user action (passphrase input), and this is then followed with another attempt from scratch to "open" the license (see this->DecryptLicense(lic, pass)), and if successful then ePub3::Container::OpenContainer() is invoked all over again (total inversion of control, 2 separate phases).

            } else if (Status::IsSuccess(result)) {
                //ASSERT (license->Decrypted())

                result = Status(StatusCode::ErrorCommonSuccess);
            }
        }

        return result;
    }

#if !DISABLE_LSD

    // INetProviderCallback
    void LcpService::OnRequestStarted(INetRequest * request)
    {
        std::unique_lock<std::mutex> locker(m_lsdSync);
        m_lsdRequestRunning = true;
    }

    // INetProviderCallback
    void LcpService::OnRequestProgressed(INetRequest * request, float progress)
    {
        float p = progress;
        float pp = p;
    }

    // INetProviderCallback
    void LcpService::OnRequestCanceled(INetRequest * request)
    {
        std::unique_lock<std::mutex> locker(m_lsdSync);
        m_lsdRequestRunning = false;
        //m_lsdCondition.notify_one();

        //m_lsdRequestStatus = this->CheckDecrypted(m_lsdOriginalLicense);
        m_lsdLicensePromise->set_value(m_lsdOriginalLicense);
    }

    // INetProviderCallback
    void LcpService::OnRequestEnded(INetRequest * request, Status result)
    {
        std::unique_lock<std::mutex> locker(m_lsdSync);
        m_lsdRequestStatus = result;

        try
        {
            if (Status::IsSuccess(result))
            {
#if USE_MEMORY_NOT_FILE
                Status hashCheckResult = this->CheckStatusDocumentHash(new BufferRedableStreamAdapter(m_lsdStream->Buffer()));
#else
                Status hashCheckResult = this->CheckStatusDocumentHash(m_lsdFile.get());
#endif //USE_MEMORY_NOT_FILE

                if (Status::IsSuccess(hashCheckResult))
                {
                    std::string jsonStr;

#if USE_MEMORY_NOT_FILE

                    const std::vector<unsigned char> & chars = m_lsdStream->Buffer();

                    if (!chars.empty()) {
                        std::string bufferStr(chars.begin(), chars.end());

                        // C++ 11
                        std::string bufferStr_(chars.data(), chars.size());

                        // C++ 98
                        std::string bufferStr__(&chars[0], chars.size());

                    }
#else
                    int length = m_lsdFile.get()->Size();
                    unsigned char * buf = new unsigned char[length];
                    m_lsdFile.get()->Read(buf, length);
                    jsonStr.assign(reinterpret_cast<char*>(buf), length);
                    delete[] buf;
                    //m_lsdFile->delete() // TODO

#endif //USE_MEMORY_NOT_FILE


                    if (!jsonStr.empty())
                    {
                        rapidjson::Document jsonDoc;
                        if (jsonDoc.Parse<rapidjson::kParseValidateEncodingFlag>(jsonStr.data()).HasParseError())
                        {
                            throw StatusException(JsonValueReader::CreateRapidJsonError(jsonDoc.GetParseError(), jsonDoc.GetErrorOffset()));
                        }

                        if (!jsonDoc.IsObject())
                        {
                            throw StatusException(JsonValueReader::CreateRapidJsonError(rapidjson::kParseErrorValueInvalid));
                        }

                        std::string lsd_id = m_jsonReader->ReadStringCheck("id", jsonDoc);
                        std::string lsd_status = m_jsonReader->ReadStringCheck("status", jsonDoc);
                        std::string lsd_message = m_jsonReader->ReadString("message", jsonDoc);
                        int lsd_device_count = m_jsonReader->ReadInteger("device_count", jsonDoc);

                        const rapidjson::Value & jsonPotentialRights = m_jsonReader->ReadObject("potential_rights", jsonDoc);
                        const rapidjson::Value & jsonUpdated = m_jsonReader->ReadObjectCheck("updated", jsonDoc);

                        const rapidjson::Value & jsonLinks = m_jsonReader->ReadObjectCheck("links", jsonDoc);

                        if (!jsonLinks.HasMember("license"))
                        {
                            throw StatusException(Status(StatusCode::ErrorOpeningLicenseStatusDocumentNotValid, "LSD links missing license"));
                        }

                        std::string licenseToDownload;

                        auto it = jsonLinks.FindMember("license");
                        if (it != jsonLinks.MemberEnd()) {
                            if (it->value.IsArray()) {
                                for (auto arrayIt = it->value.Begin(); arrayIt != it->value.End(); ++arrayIt)
                                {
                                    std::string lsd_LinksLicenseHref = m_jsonReader->ReadStringCheck("href", *arrayIt);
                                    std::string lsd_LinksLicenseType = m_jsonReader->ReadString("type", *arrayIt);
                                    if (lsd_LinksLicenseType.empty() || lsd_LinksLicenseType == "application/vnd.readium.lcp.license.v1.0+json") {
                                        licenseToDownload = lsd_LinksLicenseHref;
                                        break;
                                    }
                                }
                            } else if (it->value.IsObject()) {

                                std::string lsd_LinksLicenseHref = m_jsonReader->ReadStringCheck("href", it->value);
                                std::string lsd_LinksLicenseType = m_jsonReader->ReadString("type", it->value);
                                if (lsd_LinksLicenseType.empty() || lsd_LinksLicenseType == "application/vnd.readium.lcp.license.v1.0+json") {
                                    licenseToDownload = lsd_LinksLicenseHref;
                                }
                            }
                        }
//
//                        rapidjson::Type type = jsonLinksLicense.GetType();
//                        if (type == rapidjson::kObjectType)
//                        {
//                        }
//                        else if (type == rapidjson::kArrayType)
//                        {
//                        }

//
//                        for (auto it = jsonLinks.MemberBegin(); it != jsonLinks.MemberEnd(); ++it)
//                        {
//                            rapidjson::Type type = it->value.GetType();
//
//                            std::string linkMember(it->name.GetString(), it->name.GetStringLength());
//
//                            if (type == rapidjson::kObjectType)
//                            {
//                                if (linkMember == "license") {
//                                    //it->value
//                                }
//                            }
//                            else if (type == rapidjson::kArrayType)
//                            {
//                                for (auto arrayIt = it->value.Begin(); arrayIt != it->value.End(); ++arrayIt)
//                                {
//                                    //*arrayIt
//                                }
//                            }
//                            else
//                            {
//                                throw StatusException(Status(StatusCode::ErrorOpeningLicenseStatusDocumentNotValid, "links object is not valid"));
//                            }
//                        }


                        // TODO: download updated LCP licence if necessary (yet another async operation, this time with more complex HTTP request)
                        //m_lsdNewLcpLicenseString
                        m_lsdNewLcpLicenseString = m_lsdOriginalLicense->OriginalContent(); // TODO: remove this, just for testing!

                        if (!m_lsdNewLcpLicenseString.empty())
                        {
                            std::stringstream licenseStream(m_lsdNewLcpLicenseString);
                            std::string publicationPath(m_publicationPath.c_str());
                            ZipFile::AddFile(publicationPath, licenseStream, LcpLicensePath);

                            m_lsdRequestStatus = this->OpenLicense("", m_lsdNewLcpLicenseString, *m_lsdLicensePromise);

                            m_lsdRequestRunning = false;
                            //m_lsdCondition.notify_one();
                            //m_lsdRequestStatus = this->CheckDecrypted(newLicense);
                            //m_lsdLicensePromise->set_value(newLicense);
                            return;
                        }
                    }
                }
                else
                {
                    m_lsdRequestStatus = hashCheckResult;
                }
            }

            m_lsdRequestRunning = false;
            //m_lsdCondition.notify_one();

            //m_lsdRequestStatus = this->CheckDecrypted(m_lsdOriginalLicense);
            m_lsdLicensePromise->set_value(m_lsdOriginalLicense);
        }
        catch (const std::exception & ex)
        {
            m_lsdRequestStatus = Status(StatusCode::ErrorNetworkingRequestFailed, ex.what());

            m_lsdRequestRunning = false;
            //m_lsdCondition.notify_one();

            //m_lsdRequestStatus = this->CheckDecrypted(m_lsdOriginalLicense);
            m_lsdLicensePromise->set_value(m_lsdOriginalLicense);
        }
    }

    Status LcpService::CheckStatusDocumentHash(IReadableStream* stream)
    {
        //m_lsdLink.hash = "abcdefg"; // TODO: remove this! (just testing)

        if (!m_lsdLink.hash.empty())
        {
            std::vector<unsigned char> rawHash;
            Status res = m_cryptoProvider->CalculateFileHash(stream, rawHash);
            if (!Status::IsSuccess(res))
                return res;

            std::string hexHash;
            res = m_cryptoProvider->ConvertRawToHex(rawHash, hexHash);
            if (!Status::IsSuccess(res))
                return res;

            if (hexHash != m_lsdLink.hash)
            {
                return Status(StatusCode::ErrorStatusDocumentHashCheckFail);
            }
        }
        return Status(StatusCode::ErrorCommonSuccess);
    }

    // https://github.com/drminside/lsd_client/blob/master/lsd_client.py#L453
    std::string LcpService::ResolveTemplatedURL(const std::string & url)
    {
        // TODO
        return url;
    }

    // https://docs.google.com/document/d/1ErBf0Gl32jNH-QVKWpGPfZDMWMeQP7dH9YY5g7agguQ
    // https://docs.google.com/document/d/1VKkAG9aKbKLQYnSjSa5-_cOgnVFsgU9a7bHO0-IkRgk
    // https://docs.google.com/document/d/1oNfXQZRSGqwpexLrhw0-0a2taEvVDXS9cPs8oBKLb0U
    Status LcpService::ProcessLicenseStatusDocument(ILicense** license, std::promise<ILicense*> & licensePromise)
    {
        try
        {
            if (license == nullptr)
            {
                throw std::invalid_argument("license pointer is nullptr");
            }
            if (*license == nullptr)
            {
                throw std::invalid_argument("license is nullptr");
            }

            m_lsdOriginalLicense = *license;
            m_lsdLicensePromise = & licensePromise;

            // if (!*license->Decrypted())
            // {
            //     return Status(StatusCode::ErrorDecryptionLicenseEncrypted);
            // }

            // RootLcpNode * rootNode = dynamic_cast<RootLcpNode *>(*license);
            // if (rootNode == nullptr)
            // {
            //     throw std::logic_error("Can not cast ILicense to ILcpNode / RootLcpNode");
            // }
            
            ILinks* links = (*license)->Links();

            //bool foundLink = links->GetLink(Status, m_lsdLink);
            //if (!foundLink) {
            if (!links->Has(StatusDocument))
            {
                //return Status(StatusCode::ErrorStatusDocumentNoStatusLink);
                return Status(StatusCode::ErrorCommonSuccess); // no LSD, noop
            }

            links->GetLink(StatusDocument, m_lsdLink);
            if (m_lsdLink.type != StatusType)
            {
                return Status(StatusCode::ErrorStatusDocumentWrongType);
            }

            if (!m_lsdLink.href.length())
            {
                return Status(StatusCode::ErrorStatusDocumentInvalidUri);
            }

            std::string url = m_lsdLink.href;
            if (m_lsdLink.templated)
            {
                url = this->ResolveTemplatedURL(url);
            }

            if (m_netProvider == nullptr)
            {
                return Status(StatusCode::ErrorCommonNoNetProvider);
            }
        
            std::unique_lock<std::mutex> locker(m_lsdSync);
#if USE_MEMORY_NOT_FILE
            m_lsdStream.reset(new SimpleMemoryWritableStream());
            m_lsdRequest.reset(new DownloadInMemoryRequest(m_lsdLink.href, m_lsdStream.get()));
#else
            std::string lsdPath(m_publicationPath.c_str());
            lsdPath = lsdPath + "_TEMP.lsd";
            m_lsdFile.reset(m_fileSystemProvider->GetFile(lsdPath));
            if (m_lsdFile.get() == nullptr)
            {
                return Status(StatusCode::ErrorAcquisitionInvalidFilePath);
            }
            m_lsdRequest.reset(new DownloadInFileRequest(m_lsdLink.href, m_lsdFile.get()));
#endif //USE_MEMORY_NOT_FILE

            locker.unlock();

            m_lsdRequestStatus = Status(StatusCode::ErrorNetworkingRequestFailed);

            m_netProvider->StartDownloadRequest(m_lsdRequest.get(), this);
            m_lsdRequestRunning = true;

//            m_lsdCondition.wait(locker, [&]() { return !m_lsdRequestRunning; });
//            if (!Status::IsSuccess(m_lsdRequestStatus) || m_lsdRequest->Canceled())
//            {
//                // TODO
//            }

            //m_lsdRequestStatus = this->CheckDecrypted(newLicense);
            //licensePromise.set_value(*newLicense);
            return Status(StatusCode::ErrorStatusDocumentNewLicense);
        }
        catch (const StatusException & ex)
        {
            return ex.ResultStatus();
        }
    }

#endif //!DISABLE_LSD

    Status LcpService::DecryptLicense(ILicense * license, const std::string & userPassphrase)
    {
        try
        {
            if (license == nullptr)
            {
                throw std::invalid_argument("license is nullptr");
            }

            KeyType userKey;
            Status res = m_cryptoProvider->DecryptUserKey(userPassphrase, license, userKey);
            if (!Status::IsSuccess(res))
                return res;

            res = this->DecryptLicenseByUserKey(license, userKey);
            if (!Status::IsSuccess(res))
                return res;

            return this->AddDecryptedUserKey(license, userKey);
        }
        catch (const StatusException & ex)
        {
            return ex.ResultStatus();
        }
    }

    Status LcpService::DecryptLicenseByUserKey(ILicense * license, const KeyType & userKey)
    {
        KeyType contentKey;
        Status res = m_cryptoProvider->DecryptContentKey(userKey, license, contentKey);
        if (!Status::IsSuccess(res))
            return res;

        RootLcpNode * rootNode = dynamic_cast<RootLcpNode *>(license);
        if (rootNode == nullptr)
        {
            throw std::logic_error("Can not cast ILicense to ILcpNode");
        }

        rootNode->SetKeyProvider(std::unique_ptr<IKeyProvider>(new SimpleKeyProvider(userKey, contentKey)));
        return rootNode->DecryptNode(license, rootNode, m_cryptoProvider.get());
    }

    Status LcpService::DecryptLicenseByHexUserKey(ILicense * license, const std::string & hexUserKey)
    {
        KeyType userKey;
        Status res = m_cryptoProvider->ConvertHexToRaw(hexUserKey, userKey);
        if (!Status::IsSuccess(res))
            return res;
        return this->DecryptLicenseByUserKey(license, userKey);
    }

    Status LcpService::DecryptLicenseOnOpening(ILicense * license)
    {
        Status res = this->DecryptLicenseByStorage(license);
        if (Status::IsSuccess(res))
        {
            m_rightsService->SyncRightsFromStorage(license);
        }
        // if (Status::IsSuccess(res) || res.Code == StatusCode::ErrorDecryptionLicenseEncrypted)
        //     return Status(StatusCode::ErrorCommonSuccess);
        return res;
    }

    Status LcpService::AddDecryptedUserKey(ILicense * license, const KeyType & userKey)
    {
        std::string hexUserKey;
        Status res = m_cryptoProvider->ConvertRawToHex(userKey, hexUserKey);
        if (!Status::IsSuccess(res))
            return res;

        std::string userId = license->User()->Id();
        if (userId.empty())
        {
            userId = UnknownUserId;
        }
        return this->AddUserKey(hexUserKey, userId, license->Provider(), license->Id());
    }

    Status LcpService::DecryptLicenseByStorage(ILicense * license)
    {
        if (m_storageProvider == nullptr)
        {
            return Status(StatusCode::ErrorCommonNoStorageProvider);
        }

        std::string userKeyHex = m_storageProvider->GetValue(
            UserKeysVaultId,
            BuildStorageProviderKey(license)
            );
        if (!userKeyHex.empty())
        {
            Status res = this->DecryptLicenseByHexUserKey(license, userKeyHex);
            if (Status::IsSuccess(res)) {
                return res;
            }
        }

        std::unique_ptr<KvStringsIterator> it(m_storageProvider->EnumerateVault(UserKeysVaultId));
        for (it->First(); !it->IsDone(); it->Next())
        {
            Status res = this->DecryptLicenseByHexUserKey(license, it->Current());
            if (Status::IsSuccess(res))
                return res;
        }
        return Status(StatusCode::ErrorDecryptionLicenseEncrypted);
    }

    Status LcpService::DecryptData(
        ILicense * license,
        const unsigned char * data,
        const size_t dataLength,
        unsigned char * decryptedData,
        size_t * decryptedDataLength,
        const std::string & algorithm
        )
    {
        try
        {
            if (license == nullptr)
            {
                throw std::invalid_argument("license is nullptr");
            }

            if (!license->Decrypted())
            {
                return Status(StatusCode::ErrorDecryptionLicenseEncrypted);
            }
            
            IEncryptionProfile * profile = m_encryptionProfilesManager->GetProfile(license->Crypto()->EncryptionProfile());
            if (profile == nullptr)
            {
                return Status(StatusCode::ErrorCommonEncryptionProfileNotFound);
            }
            if (algorithm != profile->PublicationAlgorithm())
            {
                return Status(StatusCode::ErrorCommonAlgorithmMismatch);
            }

            IKeyProvider * keyProvider = dynamic_cast<IKeyProvider *>(license);
            if (keyProvider == nullptr)
            {
                throw std::logic_error("Can not cast ILicense to IKeyProvider");
            }

            return m_cryptoProvider->DecryptPublicationData(
                license,
                keyProvider,
                data,
                dataLength,
                decryptedData,
                decryptedDataLength
                );
        }
        catch (const StatusException & ex)
        {
            return ex.ResultStatus();
        }
    }

    Status LcpService::CreateEncryptedDataStream(
        ILicense * license,
        IReadableStream * stream,
        const std::string & algorithm,
        IEncryptedStream ** encStream
        )
    {
        try
        {
            if (license == nullptr || encStream == nullptr)
            {
                throw std::invalid_argument("wrong input params");
            }

            if (!license->Decrypted())
            {
                return Status(StatusCode::ErrorDecryptionLicenseEncrypted);
            }

            IEncryptionProfile * profile = m_encryptionProfilesManager->GetProfile(license->Crypto()->EncryptionProfile());
            if (profile == nullptr)
            {
                return Status(StatusCode::ErrorCommonEncryptionProfileNotFound);
            }
            if (algorithm != profile->PublicationAlgorithm())
            {
                return Status(StatusCode::ErrorCommonAlgorithmMismatch);
            }

            IKeyProvider * keyProvider = dynamic_cast<IKeyProvider *>(license);
            if (keyProvider == nullptr)
            {
                throw std::logic_error("Can not cast ILicense to IKeyProvider");
            }

            return m_cryptoProvider->CreateEncryptedPublicationStream(
                license,
                keyProvider,
                stream,
                encStream
                );
        }
        catch (const StatusException & ex)
        {
            return ex.ResultStatus();
        }
    }

    Status LcpService::AddUserKey(const std::string & userKey)
    {
        try
        {
            return this->AddUserKey(userKey, UnknownUserId, UnknownProvider);
        }
        catch (const StatusException & ex)
        {
            return ex.ResultStatus();
        }
    }

    Status LcpService::AddUserKey(
        const std::string & userKey,
        const std::string & userId,
        const std::string & providerId
        )
    {
        try
        {
            std::string randomLicenseId;
            Status res = m_cryptoProvider->GenerateUuid(randomLicenseId);
            if (!Status::IsSuccess(res))
            {
                return res;
            }
            return this->AddUserKey(userKey, userId, providerId, randomLicenseId);
        }
        catch (const StatusException & ex)
        {
            return ex.ResultStatus();
        }
    }

    Status LcpService::AddUserKey(
        const std::string & userKey,
        const std::string & userId,
        const std::string & providerId,
        const std::string & licenseId
        )
    {
        try
        {
            if (m_storageProvider == nullptr)
            {
                return Status(StatusCode::ErrorCommonNoStorageProvider);
            }

            m_storageProvider->SetValue(
                UserKeysVaultId,
                BuildStorageProviderKey(providerId, userId, licenseId),
                userKey
                );

            return Status(StatusCode::ErrorCommonSuccess);
        }
        catch (const StatusException & ex)
        {
            return ex.ResultStatus();
        }
    }

    Status LcpService::CreatePublicationAcquisition(
        const std::string & publicationPath,
        ILicense * license,
        IAcquisition ** acquisition
        )
    {
        try
        {
            if (acquisition == nullptr)
            {
                throw std::invalid_argument("acquisition is nullptr");
            }
            if (m_netProvider == nullptr)
            {
                return Status(StatusCode::ErrorCommonNoNetProvider);
            }
            *acquisition = new Acquisition(
                license,
                m_fileSystemProvider,
                m_netProvider,
                m_cryptoProvider.get(),
                publicationPath
                );
            return Status(StatusCode::ErrorCommonSuccess);
        }
        catch (const StatusException & ex)
        {
            return ex.ResultStatus();
        }
    }

    IRightsService * LcpService::GetRightsService() const
    {
        return m_rightsService.get();
    }

    std::string LcpService::RootCertificate() const
    {
        return m_rootCertificate;
    }

    INetProvider * LcpService::NetProvider() const
    {
        return m_netProvider;
    }

    IStorageProvider * LcpService::StorageProvider() const
    {
        return m_storageProvider;
    }

    IFileSystemProvider * LcpService::FileSystemProvider() const
    {
        return m_fileSystemProvider;
    }

    bool LcpService::FindLicense(const std::string & canonicalJson, ILicense ** license)
    {
        std::unique_lock<std::mutex> locker(m_licensesSync);

        auto licIt = m_licenses.find(canonicalJson);
        if (licIt != m_licenses.end())
        {
            *license = licIt->second.get();
            return true;
        }
        return false;
    }

    std::string LcpService::CalculateCanonicalForm(const std::string & licenseJson)
    {
        JsonCanonicalizer canonicalizer(licenseJson, m_jsonReader.get());
        return canonicalizer.CanonicalLicense();
    }

    std::string LcpService::BuildStorageProviderKey(
        const std::string & providerId,
        const std::string & userId,
        const std::string & licenseId
        )
    {
        std::stringstream keyStream;
        keyStream << providerId << "@" << userId << "@" << licenseId;
        return keyStream.str();
    }

    std::string LcpService::BuildStorageProviderKey(ILicense * license)
    {
        std::string userId = license->User()->Id();
        if (userId.empty())
        {
            userId = UnknownUserId;
        }
        std::stringstream keyStream;
        keyStream << license->Provider() << "@" << userId << "@" << license->Id();
        return keyStream.str();
    }
}
