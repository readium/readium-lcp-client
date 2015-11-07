#ifndef __I_LICENSE_ALT__
#define __I_LICENSE_ALT__

#include <string>

namespace lcp
{
    class ICrypto;
    class ILinks;
    class IUser;
    class IRights;

    class ILicense
    {
    public:
        virtual std::string Id() const = 0;
        virtual std::string Content() const = 0;
        virtual std::string Issued() const = 0;
        virtual std::string Updated() const = 0;
        virtual std::string Provider() const = 0;

        virtual ICrypto * Crypto() const = 0;
        virtual ILinks * Links() const = 0;
        virtual IUser * User() const = 0;
        virtual IRights * Rights() const = 0;

        virtual ~ILicense() {};
    };
}

#endif //__I_LICENSE_ALT__
