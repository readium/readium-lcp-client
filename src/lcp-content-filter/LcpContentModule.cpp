//
// Created by clebeaupin on 29/03/16.
//

#if FEATURES_READIUM

#include "public/LcpContentModule.h"
#include <public/LcpContentFilter.h>

READIUM_INCLUDE_START
#include <ePub3/container.h>
#include <ePub3/content_module_manager.h>
#include <resource_stream.h>
READIUM_INCLUDE_END

#if DEBUG
#define LOG(msg) std::cout << "LCP: " << msg << std::endl
#else
#define LOG(msg)
#endif

namespace lcp {
    ePub3::string LcpContentModule::GetModuleName() {
        return ePub3::string("lcp");
    }

    void LcpContentModule::RegisterContentFilters() {
        LcpContentFilter::Register(lcpService);
    }

    async_result<ContainerPtr> LcpContentModule::ProcessFile(const ePub3::string &path, ePub3::launch policy) {
        ContainerPtr container = Container::OpenContainerForContentModule(path);

        if(container == nullptr) {
            // Unable to open container
            return make_ready_future<ContainerPtr>(ContainerPtr(nullptr));
        }

        // Search for a lcpl file in META-INF
        bool licenseExists = container->FileExistsAtPath("META-INF/license.lcpl");

        if (!licenseExists) {
            return make_ready_future<ContainerPtr>(ContainerPtr(nullptr));
        }

        // Open container and decrypt license asynchronously
        return (async_result<ContainerPtr>) ePub3::async(
                ePub3::launch::deferred, &LcpContentModule::DecryptContainer, container);
    }

    static ContainerPtr LcpContentModule::DecryptContainer(ContainerPtr container) {
        // Read lcpl JSON file
        unique_ptr<ePub3::ByteStream> byteStream = container->ReadStreamAtPath("META-INF/license.lcpl");

        if (byteStream == nullptr) {
            return ContainerPtr(nullptr);
        }

        char *buffer = nullptr;
        std::size_t bufferSize = byteStream->ReadAllBytes((void **) &buffer);
        byteStream->Close();

        if (buffer == nullptr) {
            return ContainerPtr(nullptr);
        }

        std::string licenseJson(buffer, bufferSize);
        free(buffer);

        // Open license
        lcp::ILicense * license = nullptr;;
        lcp::Status status = lcpService->OpenLicense(licenseJson, &license);

        if (status.Code  != lcp::StatusCode::ErrorCommonSuccess) {
            // Maybe raises an error
            return ContainerPtr(nullptr);
        }

        if (!license->Decrypted()) {
            // Unable to decrypt license so call the credential handler
            credentialHandler->decrypt(license);
            return ContainerPtr(nullptr);
        }

        return ContainerPtr(container);
    }

    async_result<bool> LcpContentModule::ApproveUserAction(const UserAction &action) {
        async_result<bool> result;
        return result;
    }

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
