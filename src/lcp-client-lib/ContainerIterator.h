//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#ifndef __CONTAINER_ITERATOR_H__
#define __CONTAINER_ITERATOR_H__

#include <map>
#include <string>
#include <stdexcept>
#include "Public/IValueIterator.h"

namespace lcp
{
    template<typename ValueType, typename Container>
    class LinearContainerIterator : public IValueIterator<ValueType>
    {
    protected:
        const Container & m_container;
        typename Container::const_iterator m_current;

    public:
        explicit LinearContainerIterator(const Container & container)
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

        virtual const ValueType & Current() const
        {
            if (IsDone())
            {
                throw std::out_of_range("Iterator is out of range");
            }
            return *m_current;
        }
    };

    template<typename KeyType, typename ValueType, typename Container>
    class AssociativeIterator : public IKeyValueIterator<KeyType, ValueType>
    {
    protected:
        const Container & m_container;
        typename Container::const_iterator m_current;

    public:
        explicit AssociativeIterator(const Container & container)
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

        virtual KeyType CurrentKey() const
        {
            if (IsDone())
            {
                throw std::out_of_range("Iterator is out of range");
            }
            return m_current->first;
        }

        virtual const ValueType & Current() const
        {
            if (IsDone())
            {
                throw std::out_of_range("Iterator is out of range");
            }
            return m_current->second;
        }
    };

    template<typename ValueType>
    using MapIterator = AssociativeIterator<std::string, ValueType, std::map<std::string, ValueType> >;

    template<typename ValueType>
    using MultiMapIterator = AssociativeIterator<std::string, ValueType, std::multimap<std::string, ValueType> >;
}

#endif //__CONTAINER_ITERATOR_H__
