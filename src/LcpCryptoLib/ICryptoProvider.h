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

#ifndef __I_CRYPTO_PROVIDER_H__
#define __I_CRYPTO_PROVIDER_H__

#include "Public/LcpStatus.h"
#include "LcpUtils.h"

namespace lcp
{
    class ILicense;
    class IKeyProvider;
    class IReadableStream;
    class IEncryptedStream;

    class ICryptoProvider
    {
    public:
        virtual Status VerifyLicense(
            const std::string & rootCertificateBase64,
            ILicense * license
            ) = 0;

        virtual Status DecryptUserKey(
            const std::string & userPassphrase,
            ILicense * license,
            KeyType & userKey
            ) = 0;

        virtual Status DecryptContentKey(
            const KeyType & userKey,
            ILicense * license,
            KeyType & contentKey
            ) = 0;

        virtual Status CalculateFileHash(
            IReadableStream * readableStream,
            std::vector<unsigned char> & rawHash
            ) = 0;

        virtual Status ConvertRawToHex(
            const std::vector<unsigned char> & data,
            std::string & hex
            ) = 0;

        virtual Status ConvertHexToRaw(
            const std::string & hex,
            std::vector<unsigned char> & key
            ) = 0;

        virtual Status GenerateUuid(std::string & uuid) = 0;

        virtual Status DecryptLicenseData(
            const std::string & dataBase64,
            ILicense * license,
            IKeyProvider * keyProvider,
            std::string & decrypted
            ) = 0;

        virtual Status DecryptPublicationData(
            ILicense * license,
            IKeyProvider * keyProvider,
            const unsigned char * data,
            const size_t dataLength,
            unsigned char * decryptedData,
            size_t inDecryptedDataLength,
            size_t * outDecryptedDataLength
            ) = 0;

        virtual Status CreateEncryptedPublicationStream(
            ILicense * license,
            IKeyProvider * keyProvider,
            IReadableStream * stream,
            IEncryptedStream ** encStream
            ) = 0;

        virtual ~ICryptoProvider() {}
    };
}

#endif //__I_CRYPTO_PROVIDER_H__