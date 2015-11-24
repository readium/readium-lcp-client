#ifndef __LCP_ITERATOR_H__
#define __LCP_ITERATOR_H__

#include <map>
#include <string>

namespace lcp
{
    template<typename Container>
    class Iterator
    {
    public:
        explicit Iterator(const Container & container)
            : m_container(container)
        {
            m_current = m_container.cbegin();
        }

        void First()
        {
            m_current = m_container.cbegin();
        }

        void Next()
        {
            ++m_current;
        }

        bool IsDone() const
        {
            return (m_current == m_container.end());
        }

        typename const Container::value_type * Current() const
        {
            if (!IsDone())
            {
                throw std::out_of_range("Iterator is out of range");
            }
            return &(*m_current);
        }

    private:
        const Container & m_container;
        typename Container::const_iterator m_current;
    };

    template<typename Value>
    using MapIterator = typename Iterator<std::map<std::string, Value> >;

    template<typename Value>
    using MultiMapIterator = typename Iterator<std::multimap<std::string, Value> >;
}

#endif //__LCP_ITERATOR_H__
