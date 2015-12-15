//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright © 2015 Mantano. All rights reserved.
//  Any commercial use is strictly prohibited.
//

#include <gtest/gtest.h>

namespace lcptest
{
    template<typename T>
    bool ArraysMatch(
        const T(&expected),
        const T(&actual))
    {
        if (expected.size() != actual.size())
        {
            return ::testing::AssertionFailure();
        }

        auto expIt = expected.begin();
        auto actIt = actual.begin();
        for ( ; expIt != expected.end(), actIt != actual.end(); ++expIt, ++actIt)
        {
            if (*expIt != *actIt)
            {
                return ::testing::AssertionFailure() << "actual"
                    << " (" << *actIt << ") != expected"
                    << " (" << *expIt << ")";
            }
        }

        return ::testing::AssertionSuccess();
    }
}
