#ifndef __CONTAINER_ITERATOR_H__
#define __CONTAINER_ITERATOR_H__

#include <map>
#include <string>
#include <stdexcept>
#include "IValueIterator.h"

namespace lcp
{
    template<typename ValueType, typename Container>
    class ContainerIterator : public IValueIterator<ValueType>
    {
    protected:
        const Container & m_container;
        typename Container::const_iterator m_current;

    public:
        explicit ContainerIterator(const Container & container)
            : m_container(container)
        {
            m_current = m_container.cbegin();
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
            return (m_current == m_container.cend());
        }
    };

    template<typename ValueType, typename Container>
    class LinearContainerIterator : public ContainerIterator<ValueType, Container>
    {
    public:
        explicit LinearContainerIterator(const Container & container)
            : ContainerIterator<ValueType, Container>(container)
        {
        }

        virtual const ValueType & Current() const
        {
            if (ContainerIterator<ValueType, Container>::IsDone())
            {
                throw std::out_of_range("Iterator is out of range");
            }
            return *ContainerIterator<ValueType, Container>::m_current;
        }
    };

    template<typename ValueType, typename Container>
    class AssociativeIterator : public ContainerIterator<ValueType, Container>
    {
    public:
        explicit AssociativeIterator(const Container & container)
            : ContainerIterator<ValueType, Container>(container)
        {
        }

        virtual const ValueType & Current() const
        {
            if (ContainerIterator<ValueType, Container>::IsDone())
            {
                throw std::out_of_range("Iterator is out of range");
            }
            return ContainerIterator<ValueType, Container>::m_current->second;
        }
    };

    template<typename ValueType>
    using MapIterator = AssociativeIterator<ValueType, std::map<std::string, ValueType> >;

    template<typename ValueType>
    using MultiMapIterator = AssociativeIterator<ValueType, std::multimap<std::string, ValueType> >;
}

#endif //__CONTAINER_ITERATOR_H__
