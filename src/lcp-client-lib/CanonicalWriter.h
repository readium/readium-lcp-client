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