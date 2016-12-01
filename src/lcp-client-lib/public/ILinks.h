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

#ifndef __I_LCP_LINKS_H__
#define __I_LCP_LINKS_H__

#include <vector>
#include <string>
#include "LcpStatus.h"
#include "IValueIterator.h"

namespace lcp
{
    //
    // Represents a single link in a License document.
    //
    struct Link
    {
        Link()
        {
            templated = false;
            length = 0;
        }
        
        //
        // Link location, as an URI or URI template.
        //
        std::string href;
        
        //
        // Title of the link.
        //
        std::string title;
        
        //
        // Expected MIME media type value for the external resources.
        //
        std::string type;
        
        //
        // SHA-256 hash of the resource, to check its integrity.
        //
        std::string hash;
        
        //
        // Indicates that the href is an URI template.
        //
        bool templated;
        
        //
        // Content length in bytes.
        //
        int length;
    };

    //
    // Collection of Links available in a License.
    // Maps link names (or relations, eg. "publication") to Link instances.
    //
    class ILinks
    {
    public:
        //
        // Enumerates over all found links.
        //
        virtual IKeyValueIterator<std::string, Link> * Enumerate() const = 0;

        //
        // Returns whether there is any link with the given name (relation).
        //
        virtual bool Has(const std::string & name) const = 0;
        
        //
        // Gets the link with matching name.
        //
        virtual bool GetLink(const std::string & name, Link & link) const = 0;
        
        //
        // Returns whether there are multiple links with the given name
        // (relation).
        //
        virtual bool HasMany(const std::string & name) const = 0;
        
        //
        // Returns the list of links with matching name.
        //
        virtual bool GetLinks(const std::string & name, std::vector<Link> & links) const = 0;
        
        virtual ~ILinks() {}
    };

    //
    // Predefined LCP relations (link names):
    //
    // Location where the publication can be downloaded.
    static const char * Publication = "publication";
    // Location where a Reading System can redirect a User looking
    // for additional information about its passphrase.
    static const char * Hint = "hint";
    // Location of the license status document
    static const char * StatusDocument = "status";
    // Conveys an identifier for the link's context.
    static const char * Self = "self";
}

#endif //__I_LCP_LINKS_H__
