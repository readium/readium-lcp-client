//
// Created by clebeaupin on 29/03/16.
//

#if FEATURES_READIUM

#include "IncludeMacros.h"

#include "public/LcpContentModule.h"
#include "public/LcpContentFilter.h"

READIUM_INCLUDE_START
#include <ePub3/container.h>
#include <ePub3/content_module_exception.h>
#include <ePub3/content_module_manager.h>
#include <ePub3/utilities/byte_stream.h>
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
        LcpContentFilter::Register(lcpService, license);
    }

    async_result<ContainerPtr> LcpContentModule::ProcessFile(const ePub3::string &path, ePub3::launch policy) {
        ContainerPtr container = Container::OpenContainerForContentModule(path);

        if(container == nullptr) {
            // Unable to open container
            return make_ready_future<ContainerPtr>(ContainerPtr(nullptr));
        }

        // Search for the lcpl file in META-INF
        bool licenseExists = container->FileExistsAtPath(LcpLicensePath);
        if (!licenseExists) {
            return make_ready_future<ContainerPtr>(ContainerPtr(nullptr));
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

        std::promise<ILicense*> licensePromise;
        auto licenseFuture = licensePromise.get_future();
        Status status = lcpService->OpenLicense(path, licenseJson, licensePromise);

        if (status.Code != StatusCode::ErrorCommonSuccess
            && status.Code != StatusCode::ErrorStatusDocumentNewLicense) {
            throw ePub3::ContentModuleException("Unable to initialize LCPL license");
        }

        // ...blocks until licensePromise.set_value(licensePtr)
        LcpContentModule::lcpLicense = licenseFuture.get();
        // std::future_status status = licenseFuture.wait_for(std::chrono::system_clock::duration(0));
        // if (status == std::future_status::ready) {
        //     LcpContentModule::lcpLicense = licenseFuture.get();
        // }

        if (LcpContentModule::lcpLicense == NULL) {
            throw ePub3::ContentModuleException("Unable to get LCPL license");
        }

        if (!LcpContentModule::lcpLicense->Decrypted()) {
            // decrypt license by calling the credential handler
            credentialHandler->decrypt(LcpContentModule::lcpLicense);
            throw ePub3::ContentModuleExceptionDecryptFlow("Decrypting LCPL license ...");
        }

        return make_ready_future<ContainerPtr>(ContainerPtr(container));
    }

    async_result<bool> LcpContentModule::ApproveUserAction(const UserAction &action) {
        async_result<bool> result;
        return result;
    }
    
    ILicense *LcpContentModule::lcpLicense = NULL:
    
    ILcpService *LcpContentModule::lcpService = NULL;
    ICredentialHandler *LcpContentModule::credentialHandler = NULL;

    void LcpContentModule::Register(ILcpService *const lcpService,
                                    ICredentialHandler * credentialHandler) {
        LcpContentModule::lcpService = lcpService;
        LcpContentModule::credentialHandler = credentialHandler;
        auto contentModule = std::make_shared<LcpContentModule>();
        ContentModuleManager::Instance()->RegisterContentModule(contentModule, "LcpContentModule");
    }
}

#endif // FEATURES_READIUM
