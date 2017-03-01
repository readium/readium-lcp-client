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


#ifndef __I_VALUE_ITERATOR_H__
#define __I_VALUE_ITERATOR_H__

namespace lcp
{
    //
    // Iterator interface used when enumerating values.
    //
    template<typename ValueType>
    class IValueIterator
    {
    public:
        //
        // Go to the first value.
        //
        virtual void First() = 0;

        //
        // Move the position to the next value.
        //
        virtual void Next() = 0;

        //
        // Returns whether we are at the end of the enumeration.
        //
        virtual bool IsDone() const = 0;

        //
        // Returns the value at the current position.
        //
        virtual const ValueType & Current() const = 0;

        virtual ~IValueIterator() {}
    };

    //
    // Iterator used when enumerating key-value maps.
    //
    template<typename KeyType, typename ValueType>
    class IKeyValueIterator : public IValueIterator<ValueType>
    {
    public:
        //
        // Returns the key at the current position.
        //
        virtual KeyType CurrentKey() const = 0;
    };

    typedef IKeyValueIterator<std::string, std::string> KvStringsIterator;
}

#endif //__I_VALUE_ITERATOR_H__
