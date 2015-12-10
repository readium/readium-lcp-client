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

#ifndef __LCP_UTILS_H__
#define __LCP_UTILS_H__

#include <string>
#include <map>
#include <vector>
#include "Public/LcpStatus.h"

namespace lcp
{
    class StatusException : public std::runtime_error
    {
    public:
        StatusException(const Status & status)
            : std::runtime_error(status.Extension)
            , m_status(status)
        {
        }

        Status ResultStatus() const
        {
            return m_status;
        }

    private:
        Status m_status;
    };

    bool StringToBool(const std::string & val);
    std::string BoolToString(bool val);
    std::string DoubleToExponentString(const double & value);

    void ValidateUtf8(const std::string & utf8Str);
    bool EqualsUtf8(const std::string & left, const std::string & right);
    bool LexicographicalCompareUtf8(const std::string & left, const std::string & right);

    typedef std::map<std::string, std::string> StringsMap;
    typedef std::vector<unsigned char> KeyType;
}

#endif //__LCP_UTILS_H__