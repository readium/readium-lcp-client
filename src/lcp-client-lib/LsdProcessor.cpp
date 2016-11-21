//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//
//
//#if !DISABLE_LSD
//
//#include "LsdProcessor.h"
//#include "CryptoLcpNode.h"
//#include "LinksLcpNode.h"
//#include "UserLcpNode.h"
//#include "RightsLcpNode.h"
//#include "RootLcpNode.h"
//#include "LcpUtils.h"
//#include "JsonValueReader.h"
//#include "JsonCanonicalizer.h"
//#include "EncryptionProfilesManager.h"
//#include "CryptoppCryptoProvider.h"
//#include "SimpleKeyProvider.h"
//#include "public/IStorageProvider.h"
//#if ENABLE_NET_PROVIDER
//#include "Acquisition.h"
//#endif //ENABLE_NET_PROVIDER
//#include "RightsService.h"
//
//
//#include "IncludeMacros.h"
//
//#include <regex> //REGEX_INCLUDE
//
//ZIPLIB_INCLUDE_START
//#include "ziplib/Source/ZipLib/ZipFile.h"
//ZIPLIB_INCLUDE_END
//
//#if USE_MEMORY_NOT_FILE
//#include "DownloadInMemoryRequest.h"
//#else
//#include "DownloadInFileRequest.h"
//#endif //USE_MEMORY_NOT_FILE
//
//#include "DateTime.h"
//#include "ThreadTimer.h"
//
//
//namespace lcp
//{
//    /*static*/ std::string LsdProcessor::StatusType = "application/vnd.readium.license.status.v1.0+json";
//    static std::string const LcpLicensePath = "META-INF/license.lcpl";
//
//    LsdProcessor::LsdProcessor(ILcpService * lcpService, ILicense* license, std::string publicationPath) :
//    m_lcpService(lcpService),
//    m_license(license),
//    m_publicationPath(publicationPath),
//    m_jsonReader(new JsonValueReader()),
//    m_encryptionProfilesManager(new EncryptionProfilesManager()),
//    m_cryptoProvider(new CryptoppCryptoProvider(m_encryptionProfilesManager.get()
//#if ENABLE_NET_PROVIDER
//            , m_lcpService->NetProvider()
//
//            , std::string()
//#endif //ENABLE_NET_PROVIDER
//    ))
//#if ENABLE_NET_PROVIDER
//            ,
//    m_lsdRequestStatus(Status(StatusCode::ErrorCommonSuccess))
//#endif //ENABLE_NET_PROVIDER
//    {
//    }
//
//#if ENABLE_NET_PROVIDER
//    // INetProviderCallback
//    void LsdProcessor::OnRequestStarted(INetRequest * request)
//    {
//        std::unique_lock<std::mutex> locker(m_lsdSync);
//        m_lsdRequestRunning = true;
//    }
//
//    // INetProviderCallback
//    void LsdProcessor::OnRequestProgressed(INetRequest * request, float progress)
//    {
//        float p = progress;
//        float pp = p;
//    }
//
//    // INetProviderCallback
//    void LsdProcessor::OnRequestCanceled(INetRequest * request)
//    {
//        std::unique_lock<std::mutex> locker(m_lsdSync);
//        m_lsdRequestRunning = false;
//        //m_lsdCondition.notify_one();
//
//        //m_lsdRequestStatus = this->CheckDecrypted((*m_lsdOriginalLicensePTR));
//    }
//
//    // INetProviderCallback
//    void LsdProcessor::OnRequestEnded(INetRequest * request, Status result)
//    {
//        std::unique_lock<std::mutex> locker(m_lsdSync);
//        m_lsdRequestStatus = result;
//
//        try
//        {
//            if (Status::IsSuccess(result))
//            {
//#if USE_MEMORY_NOT_FILE
//                Status hashCheckResult = this->CheckStatusDocumentHash(new BufferRedableStreamAdapter(m_lsdStream->Buffer()));
//#else
//                Status hashCheckResult = this->CheckStatusDocumentHash(m_lsdFile.get());
//#endif //USE_MEMORY_NOT_FILE
//
//                if (Status::IsSuccess(hashCheckResult))
//                {
//                    std::string jsonStr;
//
//#if USE_MEMORY_NOT_FILE
//
//                    const std::vector<unsigned char> & chars = m_lsdStream->Buffer();
//
//                    if (!chars.empty()) {
//                        std::string bufferStr(chars.begin(), chars.end());
//
//                        // C++ 11
//                        std::string bufferStr_(chars.data(), chars.size());
//
//                        // C++ 98
//                        std::string bufferStr__(&chars[0], chars.size());
//
//                    }
//#else
//                    int length = m_lsdFile.get()->Size();
//                    unsigned char * buf = new unsigned char[length];
//                    m_lsdFile.get()->Read(buf, length);
//                    jsonStr.assign(reinterpret_cast<char*>(buf), length);
//                    delete[] buf;
//                    //m_lsdFile->delete() // TODO
//
//#endif //USE_MEMORY_NOT_FILE
//
//
//                    if (!jsonStr.empty())
//                    {
//                        rapidjson::Document jsonDoc;
//                        if (jsonDoc.Parse<rapidjson::kParseValidateEncodingFlag>(jsonStr.data()).HasParseError())
//                        {
//                            throw StatusException(JsonValueReader::CreateRapidJsonError(jsonDoc.GetParseError(), jsonDoc.GetErrorOffset()));
//                        }
//
//                        if (!jsonDoc.IsObject())
//                        {
//                            throw StatusException(JsonValueReader::CreateRapidJsonError(rapidjson::kParseErrorValueInvalid));
//                        }
//
//                        std::string lsd_id = m_jsonReader->ReadStringCheck("id", jsonDoc);
//                        std::string lsd_status = m_jsonReader->ReadStringCheck("status", jsonDoc);
//                        std::string lsd_message = m_jsonReader->ReadString("message", jsonDoc);
//                        int lsd_device_count = m_jsonReader->ReadInteger("device_count", jsonDoc);
//
//                        const rapidjson::Value & jsonPotentialRights = m_jsonReader->ReadObject("potential_rights", jsonDoc);
//                        const rapidjson::Value & jsonUpdated = m_jsonReader->ReadObjectCheck("updated", jsonDoc);
//
//                        const rapidjson::Value & jsonLinks = m_jsonReader->ReadObjectCheck("links", jsonDoc);
//
//                        if (!jsonLinks.HasMember("license"))
//                        {
//                            return;
//                        }
//
//                        std::string licenseToDownload;
//
//                        auto it = jsonLinks.FindMember("license");
//                        if (it != jsonLinks.MemberEnd()) {
//                            if (it->value.IsArray()) {
//                                for (auto arrayIt = it->value.Begin(); arrayIt != it->value.End(); ++arrayIt)
//                                {
//                                    std::string lsd_LinksLicenseHref = m_jsonReader->ReadStringCheck("href", *arrayIt);
//                                    std::string lsd_LinksLicenseType = m_jsonReader->ReadString("type", *arrayIt);
//                                    if (lsd_LinksLicenseType.empty() || lsd_LinksLicenseType == "application/vnd.readium.lcp.license.v1.0+json") {
//                                        licenseToDownload = lsd_LinksLicenseHref;
//                                        break;
//                                    }
//                                }
//                            } else if (it->value.IsObject()) {
//
//                                std::string lsd_LinksLicenseHref = m_jsonReader->ReadStringCheck("href", it->value);
//                                std::string lsd_LinksLicenseType = m_jsonReader->ReadString("type", it->value);
//                                if (lsd_LinksLicenseType.empty() || lsd_LinksLicenseType == "application/vnd.readium.lcp.license.v1.0+json") {
//                                    licenseToDownload = lsd_LinksLicenseHref;
//                                }
//                            }
//                        }
////
////                        rapidjson::Type type = jsonLinksLicense.GetType();
////                        if (type == rapidjson::kObjectType)
////                        {
////                        }
////                        else if (type == rapidjson::kArrayType)
////                        {
////                        }
//
////
////                        for (auto it = jsonLinks.MemberBegin(); it != jsonLinks.MemberEnd(); ++it)
////                        {
////                            rapidjson::Type type = it->value.GetType();
////
////                            std::string linkMember(it->name.GetString(), it->name.GetStringLength());
////
////                            if (type == rapidjson::kObjectType)
////                            {
////                                if (linkMember == "license") {
////                                    //it->value
////                                }
////                            }
////                            else if (type == rapidjson::kArrayType)
////                            {
////                                for (auto arrayIt = it->value.Begin(); arrayIt != it->value.End(); ++arrayIt)
////                                {
////                                    //*arrayIt
////                                }
////                            }
////                            else
////                            {
////                                throw StatusException(Status(StatusCode::ErrorOpeningLicenseStatusDocumentNotValid, "links object is not valid"));
////                            }
////                        }
//
//
//                        // TODO: download updated LCP licence if necessary (yet another async operation, this time with more complex HTTP request)
//                        //m_lsdNewLcpLicenseString
//                        m_lsdNewLcpLicenseString = m_license->OriginalContent(); // TODO: remove this, just for testing!
//
//                        if (!m_lsdNewLcpLicenseString.empty())
//                        {
//                            std::stringstream licenseStream(m_lsdNewLcpLicenseString);
//                            std::string publicationPath(m_publicationPath.c_str());
//                            ZipFile::AddFile(publicationPath, licenseStream, LcpLicensePath);
//
//                            m_lsdRequestStatus = m_lcpService->OpenLicense("", m_lsdNewLcpLicenseString, &m_license);
//
//                            m_lsdRequestRunning = false;
//                            //m_lsdCondition.notify_one();
//                            //m_lsdRequestStatus = this->CheckDecrypted(newLicense);
//                            //m_lsdLicensePromise->set_value(newLicense);
//                            return;
//                        }
//                    }
//                }
//                else
//                {
//                    m_lsdRequestStatus = hashCheckResult;
//                }
//            }
//
//            m_lsdRequestRunning = false;
//            //m_lsdCondition.notify_one();
//
//            //m_lsdRequestStatus = this->CheckDecrypted((*m_lsdOriginalLicensePTR));
//        }
//        catch (const std::exception & ex)
//        {
//            m_lsdRequestStatus = Status(StatusCode::ErrorNetworkingRequestFailed, ex.what());
//
//            m_lsdRequestRunning = false;
//            //m_lsdCondition.notify_one();
//
//            //m_lsdRequestStatus = this->CheckDecrypted((*m_lsdOriginalLicensePTR));
//        }
//    }
//#endif //ENABLE_NET_PROVIDER
//
//    Status LsdProcessor::CheckStatusDocumentHash(IReadableStream* stream)
//    {
//        if (!m_lsdLink.hash.empty())
//        {
//            std::vector<unsigned char> rawHash;
//            Status res = m_cryptoProvider->CalculateFileHash(stream, rawHash);
//            if (!Status::IsSuccess(res))
//                return res;
//
//            std::string hexHash;
//            res = m_cryptoProvider->ConvertRawToHex(rawHash, hexHash);
//            if (!Status::IsSuccess(res))
//                return res;
//
//            if (hexHash != m_lsdLink.hash)
//            {
//                return Status(StatusCode::ErrorCommonSuccess); // failed LSD checksum, noop
//                //return Status(StatusCode::ErrorStatusDocumentHashCheckFail);
//            }
//        }
//        return Status(StatusCode::ErrorCommonSuccess);
//    }
//
//    std::string LsdProcessor::ResolveTemplatedURL(ILicense* license, const std::string & url)
//    {
//        std::string strLicenseID("{license_id}");
//        if (url.find(strLicenseID) == std::string::npos) {
//            return url;
//        }
//
//        std::regex re(strLicenseID);
//        std::stringstream ss;
////        if ( ss.tellp() > 0 )
////            ss << ' ';
//
//        ss << std::regex_replace(url.c_str(), re, license->Id());
//
//        auto url_ = ss.str();
//        return url_;
//    }
//
//    Status LsdProcessor::CheckLicenseStatusDocument(ILicense* license)
//    {
//        try
//        {
//            if (license == nullptr)
//            {
//                throw std::invalid_argument("license pointer is nullptr");
//            }
//
//            // if (!license->Decrypted())
//            // {
//            //     return Status(StatusCode::ErrorDecryptionLicenseEncrypted);
//            // }
//
//            // RootLcpNode * rootNode = dynamic_cast<RootLcpNode *>(license);
//            // if (rootNode == nullptr)
//            // {
//            //     throw std::logic_error("Can not cast ILicense to ILcpNode / RootLcpNode");
//            // }
//
//            ILinks* links = license->Links();
//
//            //bool foundLink = links->GetLink(Status, m_lsdLink);
//            //if (!foundLink) {
//            if (!links->Has(StatusDocument))
//            {
//                //return Status(StatusCode::ErrorStatusDocumentNoStatusLink);
//                return Status(StatusCode::ErrorCommonSuccess); // no LSD, noop
//            }
//
//            links->GetLink(StatusDocument, m_lsdLink);
//            if (m_lsdLink.type != StatusType)
//            {
//                return Status(StatusCode::ErrorCommonSuccess); // bogus LSD link, noop
//                //return Status(StatusCode::ErrorStatusDocumentWrongType);
//            }
//
//            if (!m_lsdLink.href.length())
//            {
//                return Status(StatusCode::ErrorCommonSuccess); // bogus LSD link, noop
//                //return Status(StatusCode::ErrorStatusDocumentInvalidUri);
//            }
//
//            std::string url = m_lsdLink.href;
//            if (m_lsdLink.templated)
//            {
//                url = this->ResolveTemplatedURL(m_license, url);
//            }
//
//#if ENABLE_NET_PROVIDER
//            if (m_lcpService->NetProvider() == nullptr)
//            {
//                return Status(StatusCode::ErrorCommonNoNetProvider);
//            }
//
//            std::unique_lock<std::mutex> locker(m_lsdSync);
//#if USE_MEMORY_NOT_FILE
//            m_lsdStream.reset(new SimpleMemoryWritableStream());
//            m_lsdRequest.reset(new DownloadInMemoryRequest(m_lsdLink.href, m_lsdStream.get()));
//#else
//            std::string lsdPath(m_publicationPath.c_str());
//            lsdPath = lsdPath + "_TEMP.lsd";
//            m_lsdFile.reset(m_lcpService->FileSystemProvider()->GetFile(lsdPath));
//            if (m_lsdFile.get() == nullptr)
//            {
//                return Status(StatusCode::ErrorAcquisitionInvalidFilePath);
//            }
//            m_lsdRequest.reset(new DownloadInFileRequest(m_lsdLink.href, m_lsdFile.get()));
//#endif //USE_MEMORY_NOT_FILE
//
//            locker.unlock();
//
//            m_lsdRequestStatus = Status(StatusCode::ErrorNetworkingRequestFailed);
//
//            m_lcpService->NetProvider()->StartDownloadRequest(m_lsdRequest.get(), this);
//            m_lsdRequestRunning = true;
//
////            m_lsdCondition.wait(locker, [&]() { return !m_lsdRequestRunning; });
////            if (!Status::IsSuccess(m_lsdRequestStatus) || m_lsdRequest->Canceled())
////            {
////                // TODO
////            }
//
//            //m_lsdRequestStatus = this->CheckDecrypted(newLicense);
//#endif //ENABLE_NET_PROVIDER
//
//            return Status(StatusCode::LicenseStatusDocumentStartProcessing);
//        }
//        catch (const StatusException & ex)
//        {
//            return Status(StatusCode::ErrorCommonSuccess); // any LSD problem, noop
//            //return ex.ResultStatus();
//        }
//    }
//}
//
//#endif //!DISABLE_LSD