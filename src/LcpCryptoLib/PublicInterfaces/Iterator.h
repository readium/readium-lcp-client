#ifndef __LCP_ITERATOR_H__
#define __LCP_ITERATOR_H__

#include <map>

namespace lcp
{
    /*template<typename T, typename Container = std::map<T> >
    class Iterator
    {
    public:
        explicit Iterator(const Container & container)
            : m_container(container)
        {
        }

        virtual void First()
        {
            m_current = m_container.cbegin();
        }

        virtual void Next()
        {
            ++m_current;
        }

        virtual bool IsDone() const
        {
            return (m_current == m_container.end());
        }

        virtual T * Current() const
        {
            if (!IsDone())
            {
                return m_current;
            }
            return &(*m_current);
        }
    private:
        const Container & m_container;
        Container::const_iterator m_current;
    };*/
}

#endif //__LCP_ITERATOR_H__
