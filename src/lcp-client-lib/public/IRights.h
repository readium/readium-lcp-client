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
    //
    // Collection of current values (static or stored) for the License rights
    // given to the User in a License.
    //
    class IRights
    {
    public:
        //
        // Enumerates over all the right values in the License.
        //
        virtual KvStringsIterator * Enumerate() const = 0;

        //
        // Returns whether the given License right has a value.
        //
        virtual bool HasRightValue(const std::string & name) const = 0;

        //
        // Gets the value for the given right in the License.
        // Returns whether the value was available.
        //
        virtual bool GetRightValue(const std::string & name, std::string & value) const = 0;

        virtual ~IRights() {}
    };

    //
    // Predefined rights from the LCP specification.
    //
    // Maximum number of pages that can be printed over the lifetime of the license.
    static const char * PrintRight = "print";
    // Maximum number of characters that can be copied to the clipboard over
    // the lifetime of the license.
    static const char * CopyRight = "copy";
    // Indication whether the Publication can be read in audio via text-to-speech.
    static const char * TtsRight = "tts";
    // Date when the license begins.
    static const char * StartRight = "start";
    // Date when the license ends.
    static const char * EndRight = "end";
}

#endif //__I_LCP_RIGHTS_H__
