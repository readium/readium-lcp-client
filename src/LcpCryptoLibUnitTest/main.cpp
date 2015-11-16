#include <gtest/gtest.h>
#include <iostream>

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int res = RUN_ALL_TESTS();
    std::cin.get();
    return res;
}