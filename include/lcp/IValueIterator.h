#ifndef __I_VALUE_ITERATOR_H__
#define __I_VALUE_ITERATOR_H__

namespace lcp
{
    template<typename ValueType>
    class IValueIterator
    {
    public:
        virtual void First() = 0;
        virtual void Next() = 0;
        virtual bool IsDone() const = 0;
        virtual const ValueType & Current() const = 0;
        virtual ~IValueIterator() {}
    };

    template<typename KeyType, typename ValueType>
    class IKeyValueIterator : public IValueIterator<ValueType>
    {
    public:
        virtual KeyType CurrentKey() const = 0;
    };

    typedef IKeyValueIterator<std::string, std::string> KvStringsIterator;
}

#endif //__I_VALUE_ITERATOR_H__
