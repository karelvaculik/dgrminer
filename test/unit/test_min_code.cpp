//
// Created by karbal on 22.2.18.
//


#include "HelperFunctions.h"
#include "common_utils.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::_;
using ::testing::Invoke;
using namespace dgrminer;

TEST(MinCode, gspanPaperExample)
{
    std::array<int, 10> alpha1 = { 0, 1, 1, 0, 0, 1, 0, 2, 0, 10 };
    std::array<int, 10> alpha2 = { 1, 2, 2, 0, 0, 2, 0, 1, 0, 20 };
    std::array<int, 10> alpha3 = { 2, 0, 1, 0, 0, 1, 0, 1, 0, 30 };
    std::array<int, 10> alpha4 = { 2, 3, 1, 0, 0, 3, 0, 3, 0, 40 };
    std::array<int, 10> alpha5 = { 3, 1, 3, 0, 0, 2, 0, 2, 0, 50 };
    std::array<int, 10> alpha6 = { 1, 4, 2, 0, 0, 4, 0, 3, 0, 60 };

    std::array<int, 8> alpha1start = { 1, 0, 0, 1, 0, 2, 0, 100 };
    std::array<int, 8> alpha2start = { 2, 0, 0, 2, 0, 1, 0, 200 };
    std::array<int, 8> alpha3start = { 1, 0, 0, 1, 0, 1, 0, 300 };
    std::array<int, 8> alpha4start = { 1, 0, 0, 3, 0, 3, 0, 400 };
    std::array<int, 8> alpha5start = { 3, 0, 0, 2, 0, 2, 0, 500 };
    std::array<int, 8> alpha6start = { 2, 0, 0, 4, 0, 3, 0, 600 };

    std::array<int, 10> beta1 = { 0, 1, 2, 0, 0, 1, 0, 1, 0, 10 };
    std::array<int, 10> beta2 = { 1, 2, 1, 0, 0, 1, 0, 1, 0, 20 };
    std::array<int, 10> beta3 = { 2, 0, 1, 0, 0, 2, 0, 2, 0, 30 };
    std::array<int, 10> beta4 = { 2, 3, 1, 0, 0, 3, 0, 3, 0, 40 };
    std::array<int, 10> beta5 = { 3, 0, 3, 0, 0, 2, 0, 2, 0, 50 };
    std::array<int, 10> beta6 = { 0, 4, 2, 0, 0, 4, 0, 3, 0, 60 };

    std::array<int, 8> beta1start = { 2, 0, 0, 1, 0, 1, 0, 100 };
    std::array<int, 8> beta2start = { 1, 0, 0, 1, 0, 1, 0, 200 };
    std::array<int, 8> beta3start = { 1, 0, 0, 2, 0, 2, 0, 300 };
    std::array<int, 8> beta4start = { 1, 0, 0, 3, 0, 3, 0, 400 };
    std::array<int, 8> beta5start = { 3, 0, 0, 2, 0, 2, 0, 500 };
    std::array<int, 8> beta6start = { 2, 0, 0, 4, 0, 3, 0, 600 };

    // THIS ONE SHOULD BE MINIMAL
    std::array<int, 10> gamma1 = { 0, 1, 1, 0, 0, 1, 0, 1, 0, 10 };
    std::array<int, 10> gamma2 = { 1, 2, 1, 0, 0, 1, 0, 2, 0, 20 };
    std::array<int, 10> gamma3 = { 2, 0, 2, 0, 0, 2, 0, 1, 0, 30 };
    std::array<int, 10> gamma4 = { 2, 3, 2, 0, 0, 2, 0, 3, 0, 40 };
    std::array<int, 10> gamma5 = { 3, 0, 3, 0, 0, 3, 0, 1, 0, 50 };
    std::array<int, 10> gamma6 = { 2, 4, 2, 0, 0, 4, 0, 3, 0, 60 };

    std::array<int, 8> gamma1start = { 1, 0, 0, 1, 0, 1, 0, 100 };
    std::array<int, 8> gamma2start = { 1, 0, 0, 1, 0, 2, 0, 200 };
    std::array<int, 8> gamma3start = { 2, 0, 0, 2, 0, 1, 0, 300 };
    std::array<int, 8> gamma4start = { 2, 0, 0, 2, 0, 3, 0, 400 };
    std::array<int, 8> gamma5start = { 3, 0, 0, 3, 0, 1, 0, 500 };
    std::array<int, 8> gamma6start = { 2, 0, 0, 4, 0, 3, 0, 600 };

    std::vector<std::array<int, 10>> alpha = { alpha1, alpha2, alpha3, alpha4, alpha5, alpha6 };
    std::vector<std::array<int, 10>> beta = { beta1, beta2, beta3, beta4, beta5, beta6 };
    std::vector<std::array<int, 10>> gamma = { gamma1, gamma2, gamma3, gamma4, gamma5, gamma6 };

    std::vector<std::array<int, 8>> allstart = { alpha1start, alpha2start, alpha3start, alpha4start, alpha5start, alpha6start,
                                                 beta1start, beta2start, beta3start, beta4start, beta5start, beta6start, gamma1start, gamma2start, gamma3start, gamma4start, gamma5start, gamma6start };
    std::vector<std::array<int, 8>> betastart = { beta1start, beta2start, beta3start, beta4start, beta5start, beta6start, gamma1start, gamma2start, gamma3start, gamma4start, gamma5start, gamma6start };
    std::vector<std::array<int, 8>> gammastart = { gamma1start, gamma2start, gamma3start, gamma4start, gamma5start, gamma6start };

    EXPECT_FALSE(is_min_code(alpha, allstart));
    EXPECT_FALSE(is_min_code(beta, allstart));
    EXPECT_TRUE(is_min_code(gamma, allstart));


    std::vector<std::array<int, 10>> alpha_minimum = compute_min_code(alpha);
    EXPECT_FALSE(equal(alpha_minimum, alpha));

    std::vector<std::array<int, 10>> beta_minimum = compute_min_code(beta);
    EXPECT_FALSE(equal(beta_minimum, beta));

    std::vector<std::array<int, 10>> gamma_minimum = compute_min_code(gamma);
    EXPECT_TRUE(equal(gamma_minimum, gamma));
    // we cannot compare gamma to other minimum computed codes (alpha, beta) as the PAT_ID differs
}


TEST(MinCode, simpleExample)
{
    // this one should be minimal
    std::array<int, 10> el_1_1 = { 0, 1, 1, -1, 1, 1, -1, 2, -1, 30 };
    std::array<int, 10> el_1_2 = { 1, 2, 2, -1, 1, 1, -1, 3, -1, 40 };
    std::array<int, 10> el_1_3 = { 0, 3, 1, -1, 1, 1, -1, 2, -1, 10 };
    std::array<int, 10> el_1_4 = { 3, 4, 2, -1, 1, 1, -1, 4, -1, 20 };
    std::vector<std::array<int, 10>> edge_list_1;
    edge_list_1 = { el_1_1 , el_1_2, el_1_3, el_1_4 };

    // this one not
    std::array<int, 10> el_2_1 = { 0, 1, 1, -1, 1, 1, -1, 2, -1, 10 };
    std::array<int, 10> el_2_2 = { 1, 2, 2, -1, 1, 1, -1, 4, -1, 20 };
    std::array<int, 10> el_2_3 = { 0, 3, 1, -1, 1, 1, -1, 2, -1, 30 };
    std::array<int, 10> el_2_4 = { 3, 4, 2, -1, 1, 1, -1, 3, -1, 40 };
    std::vector<std::array<int, 10>> edge_list_2;
    edge_list_2 = { el_2_1 , el_2_2, el_2_3, el_2_4 };

    // starting edge
    std::vector<std::array<int, 8>> starting_edges_1;
    std::array<int, 8> se_1 = { 1, -1, 1, 1, -1, 2, -1, 80 };
    starting_edges_1 = { se_1 };


    EXPECT_TRUE(is_min_code(edge_list_1, starting_edges_1));
    EXPECT_FALSE(is_min_code(edge_list_2, starting_edges_1));

    std::vector<std::array<int, 10>> edge_list_1_minimum = compute_min_code(edge_list_1);
    EXPECT_TRUE(equal(edge_list_1_minimum, edge_list_1));

    std::vector<std::array<int, 10>> edge_list_2_minimum = compute_min_code(edge_list_2);
    EXPECT_FALSE(equal(edge_list_2_minimum, edge_list_2));
    EXPECT_TRUE(equal(edge_list_2_minimum, edge_list_1));

}