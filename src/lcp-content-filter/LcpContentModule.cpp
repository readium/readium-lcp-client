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

        // Read lcpl JSON file
        unique_ptr<ePub3::ByteStream> byteStream = container->ReadStreamAtPath("META-INF/license.lcpl");

        if (byteStream == nullptr) {
            throw ePub3::ContentModuleException("Unable to read LCPL license");
        }

        char *buffer = nullptr;
        std::size_t bufferSize = byteStream->ReadAllBytes((void **) &buffer);
        byteStream->Close();

        if (buffer == nullptr) {
            throw ePub3::ContentModuleException(" LCPL license is empty");
        }

        std::string licenseJson(buffer, bufferSize);
        free(buffer);

        // Open license
        lcp::ILicense * license = nullptr;;
        lcp::Status status = lcpService->OpenLicense(licenseJson, &license);

        if (status.Code  != lcp::StatusCode::ErrorCommonSuccess) {
            throw ePub3::ContentModuleException("Unable to initialize LCPL license");
        }

        if (!license->Decrypted()) {
            // Unable to decrypt license so call the credential handler
            credentialHandler->decrypt(license);
            throw ePub3::ContentModuleException("Unable to decrypt LCPL license");
        }

        return make_ready_future<ContainerPtr>(ContainerPtr(container));
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
