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

#include "UUIDGenerator.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <cstring>

namespace lcp
{
    UUIDGenerator::UUIDGenerator()
    {
    }

    UUIDGenerator::~UUIDGenerator()
    {
    }

    void UUIDGenerator::SetTimeEpoch()
    {
        //Get the current epoch in nanoseconds from January 1 1970
        std::chrono::nanoseconds ns = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch());
        uuid.TimeEpoch = ns.count() & 0xFFFFFFFFFFFFF;
    }

    void UUIDGenerator::SetRandomNumber(std::mt19937& PRNG)
    {
        std::uniform_int_distribution<uint32_t> uint_dist_long(0, UINT32_MAX);
        uuid.RandomNumber = uint_dist_long(PRNG);
        uuid.RandomNumber <<= 32;
        uuid.RandomNumber |= uint_dist_long(PRNG);
        uuid.RandomNumber &= 0xFFFFFFFFFFFFFFF;
    }

    void UUIDGenerator::SetNodeNumber(std::mt19937& PRNG)
    {
        /*This number is currently being randomly generated. In a distributed environment this would be known before-hand */
        std::uniform_int_distribution<unsigned> uint_dist_long(0, UINT16_MAX);
        uuid.Node = uint_dist_long(PRNG) & 0xFFFF;
    }

    std::string UUIDGenerator::GenerateUUID(bool bPrettyPrint)
    {
        static std::random_device seed;
        static std::mt19937 PRNG(seed());

        //Initialize various fields of UUID structure
        SetRandomNumber(PRNG);
        SetTimeEpoch();
        SetNodeNumber(PRNG);

        std::stringstream ssUUID;
        ssUUID << std::hex << std::setfill('0');

        ssUUID << std::setw(15) << uuid.RandomNumber; // 15 hex digits = 60 bit binary number
        ssUUID << std::setw(13) << uuid.TimeEpoch;
        ssUUID << std::setw(4) << uuid.Node;

        return bPrettyPrint ? (ssUUID.str().insert(8, 1, '-').insert(13, 1, '-').insert(18, 1, '-').insert(23, 1, '-')) : ssUUID.str();
    }
}
