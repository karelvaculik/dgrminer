//
// Created by karbal on 22.2.18.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "common_utils.h"


TEST(Equal, simpleTest)
{
    EXPECT_FALSE(equal(34, 543));
    EXPECT_TRUE(equal(34, 34));

    std::vector<int> x = {3, 4, 5};
    std::vector<int> y = {3, 5, 5};
    std::vector<int> z = {3, 4, 5};
    EXPECT_FALSE(equal(x, y));
    EXPECT_TRUE(equal(x, z));

    std::vector<std::vector<int>> x2 = {{3}, {4, 5}};
    std::vector<std::vector<int>> y2 = {{3, 4}, {5}};
    std::vector<std::vector<int>> z2 = {{3}, {4, 5}};
    EXPECT_FALSE(equal(x2, y2));
    EXPECT_TRUE(equal(x2, z2));
}