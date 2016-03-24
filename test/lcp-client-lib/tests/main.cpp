//
//  Created by Artem Brazhnikov on 11/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

#include <gtest/gtest.h>
#include <iostream>

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int res = RUN_ALL_TESTS();
    std::cin.get();
    return res;
}