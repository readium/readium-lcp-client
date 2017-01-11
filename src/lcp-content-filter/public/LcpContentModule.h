//
// Created by clebeaupin on 29/03/16.
//


#ifndef __LCP_CONTENT_MODULE_H__
#define __LCP_CONTENT_MODULE_H__

#include "ILicense.h"
#include "ILcpService.h"

#include <ePub3/content_module.h>

using namespace ePub3;

namespace lcp {
    class ICredentialHandler {
    public:
        virtual void decrypt(ILicense *license) = 0;
    };

    class IStatusDocumentHandler {
    public:
        virtual void process(ILicense *license) = 0;
    };

    class LcpContentModule : public ContentModule
    {
    public:
        ePub3::string GetModuleName();
        void RegisterContentFilters();
        static void Register(ILcpService *const lcpService, ICredentialHandler * credentialHandler, IStatusDocumentHandler * statusDocumentHandler
        );

#if FUTURE_ENABLED
        async_result<ContainerPtr>
#else
        ContainerPtr
#endif //FUTURE_ENABLED
        ProcessFile(const ePub3::string &path
#if FUTURE_ENABLED
                , ePub3::launch policy
#endif //FUTURE_ENABLED
        );

#if FUTURE_ENABLED
        async_result<bool> ApproveUserAction(const UserAction &action);
#endif //FUTURE_ENABLED

    private:
        // initialized via LcpContentModule::Register(ILcpService *const service, ICredentialHandler * credentialHandler, IStatusDocumentHandler *lcpStatusDocumentHandler)
        static ILcpService *lcpService;
        static ICredentialHandler *lcpCredentialHandler;
        static IStatusDocumentHandler *lcpStatusDocumentHandler;

        static ILicense *lcpLicense;
    };
}

#endif // __LCP_CONTENT_MODULE_H__
