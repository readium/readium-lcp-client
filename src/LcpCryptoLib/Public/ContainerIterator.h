//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//  Licensed under Gnu Affero General Public License Version 3 (provided, notwithstanding this notice,
//  Readium Foundation reserves the right to license this material under a different separate license,
//  and if you have done so, the terms of that separate license control and the following references
//  to GPL do not apply).
//
//  This program is free software: you can redistribute it and/or modify it under the terms of the GNU
//  Affero General Public License as published by the Free Software Foundation, either version 3 of
//  the License, or (at your option) any later version. You should have received a copy of the GNU
//  Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef __CONTAINER_ITERATOR_H__
#define __CONTAINER_ITERATOR_H__

#include <map>
#include <string>
#include <stdexcept>
#include "IValueIterator.h"

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
