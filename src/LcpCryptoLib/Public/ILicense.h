#ifndef __I_LCP_LICENSE__
#define __I_LCP_LICENSE__

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
        // Unique identifier for this license
        virtual std::string Id() const = 0;

        // Used when validating the signature of the license.
        virtual const std::string & Content() const = 0;

        // Date when the license was first issued (ISO 8601).
        virtual std::string Issued() const = 0;

        // Date when the license was last updated (ISO 8601).
        virtual std::string Updated() const = 0;

        // Unique identifier for the Content Provider, as an URI.
        virtual std::string Provider() const = 0;

        virtual ICrypto * Crypto() const = 0;

        // ILinks maps link relation to a Link object, or a list 
        // of Link objects.
        // A link relation is a string such as "publication".
        virtual ILinks * Links() const = 0;

        // Information about the user.
        virtual IUser * User() const = 0;

        // IRights maps a Right identifier to its JSON value, if any.
        virtual IRights * Rights() const = 0;

        virtual ~ILicense() {};
    };
}

#endif //__I_LCP_LICENSE__
