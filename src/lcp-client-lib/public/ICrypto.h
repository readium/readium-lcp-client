//
//  Created by Mickaël Menu, Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#ifndef __I_LCP_CRYPTO_H__
#define __I_LCP_CRYPTO_H__

#include <string>

namespace lcp
{
    //
    // ICrypto contains the Encryption profile and info of a License Document.
    //
    class ICrypto
    {
    public:
        //
        // Encryption profile used by this LCP-protected publication,
        // as an URI (e.g. http://readium.org/lcp/profile-1.0)
        //
        virtual std::string EncryptionProfile() const = 0;

        //
        // Content Key encrypted with the User Key (base 64).
        //
        virtual std::string ContentKey() const = 0;

        //
        // Algorithm used to encrypt the Content Key, as an URI (W3C XML
        // Encryption).
        //
        virtual std::string ContentKeyAlgorithm() const = 0;

        //
        // Hint to be displayed to the User to help them remember the
        // passphrase.
        //
        virtual std::string UserKeyHint() const = 0;

        //
        // Algorithm used to generate the user key from the User 
        // Passphrase, as an URI (W3C XML Encryption).
        //
        virtual std::string UserKeyAlgorithm() const = 0;

        //
        // The value of the License's identifier field, encrypted using 
        // the User Key.
        //
        virtual std::string UserKeyCheck() const = 0;

        //
        // Value of the License signature.
        //
        virtual std::string Signature() const = 0;

        //
        // Algorithm used to calculate the signature, as an URI (W3C XML
        // Encryption).
        //
        virtual std::string SignatureAlgorithm() const = 0;

        //
        // X509 certificate used by the Content Provider.
        //
        virtual std::string SignatureCertificate() const = 0;

        virtual ~ICrypto() {}
    };
}

#endif //__I_LCP_CRYPTO_H__
