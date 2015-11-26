#ifndef __I_LCP_RIGHTS_H__
#define __I_LCP_RIGHTS_H__

#include "IValueIterator.h"

namespace lcp
{
    class IRights
    {
    public:
        virtual bool GetRightValue(const std::string & name, std::string & value) const = 0;
        //virtual Status Enumerate(Iterator<std::string> ** iterator) const = 0;
        virtual ~IRights() {}
    };

    static const char * PrintRight = "print";
    static const char * CopyRight = "copy";
    static const char * TtsRight = "tts";
    static const char * StartRight = "start";
    static const char * EndRight = "end";
}

#endif //__I_LCP_RIGHTS_H__
