#ifndef __I_RIGHTS_H__
#define __I_RIGHTS_H__

namespace lcp
{
    class IRights
    {
    public:
        virtual ~IRights() {}

    public:
        static const char * PRINT;
        static const char * COPY;
        static const char * TTS;
        static const char * START;
        static const char * END;
    };
}

#endif //__I_RIGHTS_H__
