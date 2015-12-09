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