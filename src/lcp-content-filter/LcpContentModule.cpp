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

#if FEATURES_READIUM

#include "IncludeMacros.h"

#include "public/LcpContentModule.h"
#include "public/LcpContentFilter.h"

READIUM_INCLUDE_START
#include <ePub3/container.h>
#include <ePub3/content_module_exception.h>
#include <ePub3/content_module_manager.h>
#include <ePub3/utilities/byte_stream.h>
#include <ePub3/utilities/utfstring.h>
READIUM_INCLUDE_END

#if DEBUG
#define LOG(msg) std::cout << "LCP: " << msg << std::endl
#else
#define LOG(msg)
#endif

static std::string const LcpLicensePath = "META-INF/license.lcpl";

namespace lcp {
    ePub3::string LcpContentModule::GetModuleName() {
        return ePub3::string("lcp");
    }

    void LcpContentModule::RegisterContentFilters() {

        if (LcpContentModule::lcpLicense == NULL) {
            throw ePub3::ContentModuleException("LcpContentModule::RegisterContentFilters() called before LCP license is obtained from LcpContentModule::ProcessFile()!");
        }
        LcpContentFilter::Register(LcpContentModule::lcpService, LcpContentModule::lcpLicense);
    }
#if FUTURE_ENABLED
    async_result<ContainerPtr>
#else
    ContainerPtr
#endif //FUTURE_ENABLED
    LcpContentModule::ProcessFile(const ePub3::string &path
#if FUTURE_ENABLED
            , ePub3::launch policy
#endif //FUTURE_ENABLED
                                  ) {
        ContainerPtr container = Container::OpenContainerForContentModule(path, true);

        if(container == nullptr) {

#if FUTURE_ENABLED
            return make_ready_future<ContainerPtr>(ContainerPtr(nullptr));
#else
            return nullptr;
#endif //FUTURE_ENABLED

        }

        // Search for the lcpl file in META-INF
        bool licenseExists = container->FileExistsAtPath(LcpLicensePath);
        if (!licenseExists) {

#if FUTURE_ENABLED
            return make_ready_future<ContainerPtr>(ContainerPtr(nullptr));
#else
            return nullptr;
#endif //FUTURE_ENABLED
        }

        // Read lcpl JSON file
        unique_ptr<ePub3::ByteStream> byteStream = container->ReadStreamAtPath(LcpLicensePath);
        if (byteStream == nullptr) {
            throw ePub3::ContentModuleException("Unable to read LCPL license");
        }

        char *buffer = nullptr;
        std::size_t bufferSize = byteStream->ReadAllBytes((void **) &buffer);
        byteStream->Close();

        if (buffer == nullptr) {
            throw ePub3::ContentModuleException("LCPL license is empty");
        }

        std::string licenseJson(buffer, bufferSize);
        free(buffer);

        ILicense* license = nullptr;
        ILicense** licensePTR = &license;

        Status status = LcpContentModule::lcpService->OpenLicense(
                path.stl_str(),
                licenseJson, licensePTR);

        if ((status.Code != StatusCode::ErrorCommonSuccess)
            && (status.Code != StatusCode::LicenseStatusDocumentStartProcessing)
                ) {
            std::string msg("LCP license problem: ");
            msg += Status::ToString(status);
//            if (status.Extension.length() > 0) {
//                msg += " [";
//                msg += status.Extension;
//                msg += "]";
//            }
//            msg += " (";
//            msg += status.Code;
//            msg += ")";

            throw ePub3::ContentModuleException(msg);
        }

        if ((*licensePTR) == nullptr) {
            throw ePub3::ContentModuleException("Unable to get LCPL license");
        }

        if (!(*licensePTR)->Decrypted()) {
            (*licensePTR)->setStatusDocumentProcessingFlag(false); // ensure reset of LicenseStatusDocumentStartProcessing (see below)

            // Should be non-blocking, to ensure that the exception below is captured on the app side and the main thread resumes activity to process the next events in the queue
            LcpContentModule::lcpCredentialHandler->decrypt((*licensePTR));

            // the caller gracefully degrades by interrupting the EPUB opening process (the call to credentialHandler->decrypt() above will attempt again).
            throw ePub3::ContentModuleExceptionDecryptFlow("LCPL license decrypt needs user passphrase input...");
        }

        if (status.Code == StatusCode::LicenseStatusDocumentStartProcessing) {

            // Should be non-blocking, to ensure that the exception below is captured on the app side and the main thread resumes activity to process the next events in the queue
            LcpContentModule::lcpStatusDocumentHandler->process((*licensePTR)); //LcpContentModule::lcpService, path

            // the caller gracefully degrades by interrupting the EPUB opening process (the call to lsdProcessingHandler->process() above will attempt again).
            throw ePub3::ContentModuleExceptionDecryptFlow("LCPL license status document processing...");
        }

        // NOTE: LcpContentModule::RegisterContentFilters() needs LcpContentModule::lcpLicense to be set.
        LcpContentModule::lcpLicense = (*licensePTR);

#if FUTURE_ENABLED
        return make_ready_future<ContainerPtr>(container);
#else
        return container;
#endif //FUTURE_ENABLED
    }

#if FUTURE_ENABLED
    async_result<bool> LcpContentModule::ApproveUserAction(const UserAction &action) {
        async_result<bool> result;
        return result;
    }
#endif //FUTURE_ENABLED

    // static
    ILicense *LcpContentModule::lcpLicense = NULL;

    // static
    ILcpService *LcpContentModule::lcpService = NULL;

    // static
    ICredentialHandler *LcpContentModule::lcpCredentialHandler = NULL;
    IStatusDocumentHandler *LcpContentModule::lcpStatusDocumentHandler = NULL;

    void LcpContentModule::Register(ILcpService *const service,
                                    ICredentialHandler * credentialHandler,
                                    IStatusDocumentHandler * statusDocumentHandler
    ) {
        LcpContentModule::lcpService = service;
        LcpContentModule::lcpCredentialHandler = credentialHandler;
        LcpContentModule::lcpStatusDocumentHandler = statusDocumentHandler;

        //std::shared_ptr<LcpContentModule> contentModule = std::make_shared<LcpContentModule>();
        LcpContentModule* contentModule = new LcpContentModule();

        ContentModuleManager::Instance()->RegisterContentModule(contentModule, ePub3::string("LcpContentModule")); //_NOEXCEPT
    }
}

#endif // FEATURES_READIUM
