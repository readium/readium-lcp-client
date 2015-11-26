#ifndef __I_DECRYPTION_CONTEXT_H__
#define __I_DECRYPTION_CONTEXT_H__

namespace lcp
{
    class IDecryptionContext
    {
    public:
        virtual bool IsFirstRange() const = 0;
        virtual void SetFirstRange(bool value) = 0;
        virtual ~IDecryptionContext() {}
    };
}

#endif //__I_DECRYPTION_CONTEXT_H__
