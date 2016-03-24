//
//  Created by Mickaël Menu, Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

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
    // Conveys an identifier for the link's context.
    static const char * Self = "self";
}

#endif //__I_LCP_LINKS_H__
