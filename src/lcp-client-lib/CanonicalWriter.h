//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#ifndef __CANONICAL_WRITER_H__
#define __CANONICAL_WRITER_H__

#include "rapidjson/writer.h"
#include "LcpUtils.h"

using namespace rapidjson;

namespace lcp
{
    template<typename OutputStream, typename SourceEncoding = UTF8<>, typename TargetEncoding = UTF8<>, typename StackAllocator = CrtAllocator>
    class CanonicalWriter : public Writer<OutputStream, SourceEncoding, TargetEncoding, StackAllocator> {
    public:
        typedef Writer<OutputStream, SourceEncoding, TargetEncoding, StackAllocator> Base;
        typedef typename Base::Ch Ch;

        //! Constructor
        /*! \param os Output stream.
        \param allocator User supplied allocator. If it is null, it will create a private one.
        \param levelDepth Initial capacity of stack.
        */
        CanonicalWriter(OutputStream& os, StackAllocator* allocator = 0, size_t levelDepth = Base::kDefaultLevelDepth) :
            Base(os, allocator, levelDepth) {}


        bool Double(double d)
        {
            std::string doubleStr = DoubleToExponentString(d);
            Base::Prefix(kNumberType);
            char * begin = &doubleStr.front();
            for (char* p = begin; p != begin + doubleStr.size(); ++p)
            {
                Base::os_->Put(*p);
            }
            return true;
        }

        bool String(const Ch* str, SizeType length, bool copy = false)
        {
            return Base::String(str, length, copy);
        }
        bool Key(const Ch* str, SizeType length, bool copy = false) { return Base::Key(str, length, copy); }

        bool String(const Ch* str) { return Base::String(str); }
        bool Key(const Ch* str) { return Base::Key(str); }

    private:
        // Prohibit copy constructor & assignment operator.
        CanonicalWriter(const CanonicalWriter&);
        CanonicalWriter& operator=(const CanonicalWriter&);
    };
}
#endif //__CANONICAL_WRITER_H__