//
//  Created by Mickaël Menu, Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#ifndef __I_LCP_LICENSE__
#define __I_LCP_LICENSE__

#include <string>

namespace lcp
{
    class ICrypto;
    class ILinks;
    class IUser;
    class IRights;

    //
    // Represents a single License Document, and provides access to its
    // metadata.
    //
    class ILicense
    {
    public:
        //
        // Unique identifier for this license
        //
        virtual std::string Id() const = 0;

        //
        // Canonical JSON form of the license. Used when validating the signature.
        //
        virtual std::string CanonicalContent() const = 0;

        //
        // Original JSON form of the license, given to the LcpService when
        // opening the License.
        //
        virtual std::string OriginalContent() const = 0;

        //
        // Date when the license was first issued (ISO 8601).
        //
        virtual std::string Issued() const = 0;

        //
        // Date when the license was last updated (ISO 8601).
        //
        virtual std::string Updated() const = 0;

        //
        // Unique identifier for the Content Provider, as an URI.
        //
        virtual std::string Provider() const = 0;

        //
        // Encryption Profile metadata for this License.
        //
        virtual ICrypto * Crypto() const = 0;

        //
        // List of links available in the License.
        //
        virtual ILinks * Links() const = 0;

        //
        // Information about the User.
        //
        virtual IUser * User() const = 0;

        //
        // License rights attributed to the User.
        // They are automatically synced with the consumption
        // stored on the device.
        //
        virtual IRights * Rights() const = 0;

        //
        // Returns true if the License is decrypted (ie. the passphrase
        // was given to the LcpService.
        //
        virtual bool Decrypted() const = 0;

        virtual ~ILicense() {};
    };
}

#endif //__I_LCP_LICENSE__
