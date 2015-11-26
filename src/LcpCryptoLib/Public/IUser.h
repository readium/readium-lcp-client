#ifndef __I_LCP_USER_H__
#define __I_LCP_USER_H__

#include "IValueIterator.h"

namespace lcp
{
    class IUser
    {
    public:
        virtual std::string Id() const = 0;
        virtual std::string Email() const = 0;
        virtual std::string Name() const = 0;
        virtual bool GetUserValue(const std::string & name, std::string & value) const = 0;
        //virtual Status Enumerate(Iterator<std::string> ** iterator) const = 0;
        virtual ~IUser() {}
    };
}

#endif //__I_LCP_USER_H__
