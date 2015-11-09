#ifndef __I_LCP_RIGHTS_H__
#define __I_LCP_RIGHTS_H__

#include "Iterator.h"

namespace lcp
{
    class IRights
    {
    public:
        virtual bool GetRightValue(const std::string & name, std::string & value) const = 0;
        //virtual Status Enumerate(Iterator<std::string> ** iterator) const = 0;
        virtual ~IRights() {}

    public:
        static const char * PRINT;
        static const char * COPY;
        static const char * TTS;
        static const char * START;
        static const char * END;
    };
}

#endif //__I_LCP_RIGHTS_H__
