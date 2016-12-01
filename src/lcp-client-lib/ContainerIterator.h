// Copyright (c) 2016 Mantano
// Licensed to the Readium Foundation under one or more contributor license agreements.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation and/or
//    other materials provided with the distribution.
// 3. Neither the name of the organization nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#ifndef __CONTAINER_ITERATOR_H__
#define __CONTAINER_ITERATOR_H__

#include <map>
#include <string>
#include <stdexcept>
#include "public/IValueIterator.h"

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
