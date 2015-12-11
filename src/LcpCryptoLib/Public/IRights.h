//
//  Created by Mickaël Menu, Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#ifndef __I_LCP_RIGHTS_H__
#define __I_LCP_RIGHTS_H__

#include "IValueIterator.h"

namespace lcp
{
    class IRights
    {
    public:
        virtual KvStringsIterator * Enumerate() const = 0;
        virtual bool HasRightValue(const std::string & name) const = 0;
        virtual bool GetRightValue(const std::string & name, std::string & value) const = 0;
        virtual ~IRights() {}
    };

    static const char * PrintRight = "print";
    static const char * CopyRight = "copy";
    static const char * TtsRight = "tts";
    static const char * StartRight = "start";
    static const char * EndRight = "end";
}

#endif //__I_LCP_RIGHTS_H__
