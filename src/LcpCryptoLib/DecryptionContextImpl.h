#ifndef __DECRYPTION_CONTEXT_IMPL_H__
#define __DECRYPTION_CONTEXT_IMPL_H__

#include <vector>
#include "Public/IDecryptionContext.h"

namespace lcp
{
    class DecryptionContextImpl : public IDecryptionContext
    {
    public:
        DecryptionContextImpl()
            : m_firstPart(false)
        {
        }
        virtual bool IsFirstRange() const
        {
            return m_firstPart;
        }
        virtual void SetFirstRange(bool value)
        {
            m_firstPart = value;
        }
        virtual bool HasIV() const
        {
            return !m_iv.empty();
        }
        virtual void SetIV(const std::vector<unsigned char> & iv)
        {
            m_iv = iv;
        }
        virtual std::vector<unsigned char> IV() const
        {
            return m_iv;
        }

    protected:
        bool m_firstPart;
        std::vector<unsigned char> m_iv;
    };
}

#endif //__DECRYPTION_CONTEXT_IMPL_H__
