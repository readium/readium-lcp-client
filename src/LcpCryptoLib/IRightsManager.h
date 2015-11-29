#ifndef __I_RIGHTS_MANAGER_H__
#define __I_RIGHTS_MANAGER_H__

#include <string>

namespace lcp
{
    class IRightsManager
    {
    public:
        virtual bool HasRight(const std::string & name) const = 0;
        virtual bool Consume(const std::string & name) = 0;
        virtual bool Consume(const std::string & name, int amount) = 0;
        virtual void SetRightValue(const std::string & name, const std::string & value) = 0;
        virtual ~IRightsManager() {}
    };
}

#endif //__I_RIGHTS_MANAGER_H__
