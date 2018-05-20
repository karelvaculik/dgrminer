//
// Created by karbal on 25.2.18.
//



#include "AdjacencyList.h"
#include "DGRSubgraphMining.h"
#include "HelperPrintFunctions.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <iostream>

using ::testing::_;
using ::testing::Invoke;
using namespace dgrminer;


TEST(DGRSubgraphMiningComputeAntecedentPatternFromPattern, no_isolated_elements)
{
    std::vector<std::array<int, PAT___SIZE>> pattern_edge_list = {
            {0, 1, 20, -10, 0, 101, -5, 30, -15, 0}
    };


    std::vector<std::array<int, 10>> antecedent_pattern_edges;
    std::vector<int> antecedent_pattern_isolated_node_labels;
    std::vector<int> antecedent_pattern_isolated_node_changetime;
    std::vector<int> antecedent_pattern_isolated_node_id;
    std::vector<std::array<int, 10>> antecedent_pattern_edges_ADDED;
    std::vector<int> antecedent_pattern_node_id_ADDED;
    std::vector<int> antecedent_pattern_node_labels_ADDED;

    std::vector<std::array<int, 10>> antecedent_pattern_edges_ground_truth = {{0, 1, 20, -10, 0, 101, -5, 30, -15, 0}};
    std::vector<int> antecedent_pattern_isolated_node_labels_ground_truth = {};
    std::vector<int> antecedent_pattern_isolated_node_changetime_ground_truth = {};
    std::vector<int> antecedent_pattern_isolated_node_id_ground_truth = {};
    std::vector<std::array<int, 10>> antecedent_pattern_edges_ADDED_ground_truth = {};
    std::vector<int> antecedent_pattern_node_id_ADDED_ground_truth = {};
    std::vector<int> antecedent_pattern_node_labels_ADDED_ground_truth = {};


    compute_antecedent_pattern_from_pattern(pattern_edge_list, antecedent_pattern_edges,
                                            antecedent_pattern_isolated_node_labels,
                                            antecedent_pattern_isolated_node_changetime,
                                            antecedent_pattern_isolated_node_id, antecedent_pattern_edges_ADDED,
                                            antecedent_pattern_node_id_ADDED,
                                            antecedent_pattern_node_labels_ADDED);

    ASSERT_EQ(antecedent_pattern_edges, antecedent_pattern_edges_ground_truth);
    ASSERT_EQ(antecedent_pattern_isolated_node_labels, antecedent_pattern_isolated_node_labels_ground_truth);
    ASSERT_EQ(antecedent_pattern_isolated_node_changetime, antecedent_pattern_isolated_node_changetime_ground_truth);
    ASSERT_EQ(antecedent_pattern_isolated_node_id, antecedent_pattern_isolated_node_id_ground_truth);
    ASSERT_EQ(antecedent_pattern_edges_ADDED, antecedent_pattern_edges_ADDED_ground_truth);
    ASSERT_EQ(antecedent_pattern_node_id_ADDED, antecedent_pattern_node_id_ADDED_ground_truth);
    ASSERT_EQ(antecedent_pattern_node_labels_ADDED, antecedent_pattern_node_labels_ADDED_ground_truth);
}


TEST(DGRSubgraphMiningComputeAntecedentPatternFromPattern, two_isolated_nodes)
{
    std::vector<std::array<int, PAT___SIZE>> pattern_edge_list = {
            {0, 1, 20, -10, 0, 101, 0, 30, -15, 0}
    };


    std::vector<std::array<int, 10>> antecedent_pattern_edges;
    std::vector<int> antecedent_pattern_isolated_node_labels;
    std::vector<int> antecedent_pattern_isolated_node_changetime;
    std::vector<int> antecedent_pattern_isolated_node_id;
    std::vector<std::array<int, 10>> antecedent_pattern_edges_ADDED;
    std::vector<int> antecedent_pattern_node_id_ADDED;
    std::vector<int> antecedent_pattern_node_labels_ADDED;

    std::vector<std::array<int, 10>> antecedent_pattern_edges_ground_truth = {};
    std::vector<int> antecedent_pattern_isolated_node_labels_ground_truth = {20, 30};
    std::vector<int> antecedent_pattern_isolated_node_changetime_ground_truth = {-10, -15};
    std::vector<int> antecedent_pattern_isolated_node_id_ground_truth = {0 ,1};
    std::vector<std::array<int, 10>> antecedent_pattern_edges_ADDED_ground_truth = {{0, 1, 20, -10, 0, 101, 0, 30, -15, 0}};
    std::vector<int> antecedent_pattern_node_id_ADDED_ground_truth = {};
    std::vector<int> antecedent_pattern_node_labels_ADDED_ground_truth = {};


    compute_antecedent_pattern_from_pattern(pattern_edge_list, antecedent_pattern_edges,
                                            antecedent_pattern_isolated_node_labels,
                                            antecedent_pattern_isolated_node_changetime,
                                            antecedent_pattern_isolated_node_id, antecedent_pattern_edges_ADDED,
                                            antecedent_pattern_node_id_ADDED,
                                            antecedent_pattern_node_labels_ADDED);

    ASSERT_EQ(antecedent_pattern_edges, antecedent_pattern_edges_ground_truth);
    ASSERT_EQ(antecedent_pattern_isolated_node_labels, antecedent_pattern_isolated_node_labels_ground_truth);
    ASSERT_EQ(antecedent_pattern_isolated_node_changetime, antecedent_pattern_isolated_node_changetime_ground_truth);
    ASSERT_EQ(antecedent_pattern_isolated_node_id, antecedent_pattern_isolated_node_id_ground_truth);
    ASSERT_EQ(antecedent_pattern_edges_ADDED, antecedent_pattern_edges_ADDED_ground_truth);
    ASSERT_EQ(antecedent_pattern_node_id_ADDED, antecedent_pattern_node_id_ADDED_ground_truth);
    ASSERT_EQ(antecedent_pattern_node_labels_ADDED, antecedent_pattern_node_labels_ADDED_ground_truth);
}
