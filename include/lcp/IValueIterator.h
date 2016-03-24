//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

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
