//
// Created by karbal on 22.2.18.
//

#include "HelperFunctions.h"
#include "common_utils.h"
#include "HelperPrintFunctions.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::_;
using ::testing::Invoke;
using namespace dgrminer;


TEST(HelperFunctions, test_find_maximal_subsets_1)
{
    std::set<int> set1 = { 2, 3, 1 };
    std::set<int> set2 = { 1, 2 };
    std::set<int> set3 = { 3, 2 };
    std::set<int> set4 = { 2, 4 };
    std::set<int> set5 = {};

    std::vector<std::set<int>> sets;
    sets.push_back(set1);
    sets.push_back(set2);
    sets.push_back(set3);
    sets.push_back(set4);
    sets.push_back(set5);

    std::vector<std::vector<int>> output = find_maximal_subsets(sets);

    std::vector<std::vector<int>> ground_truth = {{1, 2, 3}, {2, 4}};

    EXPECT_TRUE(equal(output, ground_truth));
}

TEST(HelperFunctions, test_find_maximal_subsets_of_pairs)
{
    int_pair pair11 = { 0,1 };
    int_pair pair12 = { 1,2 };
    int_pair pair13 = { 2,3 };

    int_pair pair21 = { 0,1 };
    int_pair pair22 = { 1,2 };

    int_pair pair31 = { 2,3 };
    int_pair pair32 = { 1,2 };

    int_pair pair41 = { 2,4 };
    int_pair pair42 = { 1,2 };

    int_pair pair51 = { 2,3 };
    int_pair pair52 = { 1,1 };

    std::set<int_pair> set1 = { pair11, pair12, pair13 };
    std::set<int_pair> set2 = { pair21, pair22 };
    std::set<int_pair> set3 = { pair31, pair32 };
    std::set<int_pair> set4 = { pair41, pair42 };
    std::set<int_pair> set5 = { pair51, pair52 };
    std::set<int_pair> set6 = {};

    std::vector<std::set<int_pair>> sets;
    sets.push_back(set1);
    sets.push_back(set2);
    sets.push_back(set3);
    sets.push_back(set4);
    sets.push_back(set5);
    sets.push_back(set6);

    std::vector<std::vector<int_pair>> result = find_maximal_subsets_of_pairs(sets);

    std::vector<std::vector<int_pair>> ground_truth = {{pair11, pair12, pair13}, {pair42, pair41}, {pair52, pair51}};

    EXPECT_TRUE(equal(result, ground_truth));

//    std::cout << "RESULT SETS (" << output.size() << "): " << std::endl;
//    for (size_t i = 0; i < output.size(); i++)
//    {
//        for (size_t j = 0; j < output[i].size(); j++)
//        {
//            std::cout << output[i][j].first << "->" << output[i][j].second << ", ";
//        }
//
//        std::cout << std::endl;
//    }
}
