#ifndef __I_DECRYPTION_CONTEXT_H__
#define __I_DECRYPTION_CONTEXT_H__

namespace lcp
{
    struct DecRangeInfo
    {
        DecRangeInfo()
            : position(0)
            , length(0)
        {}
        size_t position;
        size_t length;
    };

    class IDecryptionContext
    {
    public:
        virtual DecRangeInfo GetDecryptionRange() const = 0;
        virtual ~IDecryptionContext() {}
    };
}

#endif //__I_DECRYPTION_CONTEXT_H__
