//
// Created by karbal on 25.2.18.
//



#include "AdjacencyList.h"
#include "DGRSubgraphMining.h"
#include "HelperPrintFunctions.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::_;
using ::testing::Invoke;
using namespace dgrminer;


TEST(DGRSubgraphMiningEnumerateAntecedent, single_graph_no_separate_antecedent)
{
    // the antecedent is only in the same position as pattern

    std::vector<AdjacencyListCrate> adjacency_lists;
    std::vector<std::vector<int>> adjacencyList = {
            {1, 2},
            {0, 2},
            {0, 1}
    };
    std::array<int, ADJ_INFO___SIZE> e_info0 = {10,  -5, 0, 100, 0, 20, -10, 0}; // = flipAdjacencyInfo(newedges[e_index]
    std::array<int, ADJ_INFO___SIZE> e_info1 = {20, -10, 0, 101, 0, 20, -10, 1};
    std::array<int, ADJ_INFO___SIZE> e_info2 = {20, -10, 0, 101, 0, 10,  -5, 2};
    std::vector<std::vector<std::array<int, ADJ_INFO___SIZE>>> adjacencyEdgeInfo = {
            {e_info0, flipAdjacencyInfo(e_info2)},
            {flipAdjacencyInfo(e_info0), e_info1},
            {e_info2, flipAdjacencyInfo(e_info1)}
    };
    std::vector<std::array<int, ADJ_NODES___SIZE>> adjacencyListNodes = {
            {0,  -5, 10},
            {1, -10, 20},
            {2, -10, 20}
    };
    AdjacencyListCrate alc = AdjacencyListCrate();
    alc.adjacencyEdgeInfo = adjacencyEdgeInfo;
    alc.adjacencyList = adjacencyList;
    alc.nodes = adjacencyListNodes;
    adjacency_lists.push_back(alc);
    std::vector<int> graph_ids = {0};
    std::vector<int> antecedent_graph_ids = {0};


    // the vertices are isolated, the edge is addition edge
    std::vector<std::array<int, PAT___SIZE>> pattern_edge_list = {
            {0, 1, 10, -5, 0, 100, 0, 20, -10, 0}
    };
    // both vertices and the edge are occupied
    std::vector<std::set<int>> nodes_occupied_by_antecedent = {{0, 1}};
    std::vector<std::set<int>> edges_occupied_by_antecedent = {{0}};
    bool new_measures = false;
    std::unordered_map<int, std::set<std::set<int>>> multiple_occurrences;

    std::vector<std::array<int, 10>> antecedent_pattern_edges;
    std::vector<std::array<int, 10>> antecedent_pattern_edges_ADDED;
    std::vector<int> antecedent_pattern_isolated_node_id;
    std::vector<int> antecedent_pattern_isolated_node_labels;
    std::vector<int> antecedent_pattern_isolated_node_changetime;
    std::vector<int> antecedent_pattern_node_id_ADDED;
    std::vector<int> antecedent_pattern_node_labels_ADDED;
    compute_antecedent_pattern_from_pattern(pattern_edge_list, antecedent_pattern_edges,
                                            antecedent_pattern_isolated_node_labels,
                                            antecedent_pattern_isolated_node_changetime,
                                            antecedent_pattern_isolated_node_id, antecedent_pattern_edges_ADDED,
                                            antecedent_pattern_node_id_ADDED,
                                            antecedent_pattern_node_labels_ADDED);

    bool search_for_anomalies = false;
    bool debugging = false;
    std::set<anomaly_pattern_with_occurrences> anomaly_patterns;
    PartialUnion pu;
    pu.appendToAntecedentLabelEncoding(10, 10);
    pu.appendToAntecedentLabelEncoding(20, 20);
    pu.appendToAntecedentLabelEncoding(101, 1000);
    pu.appendToAntecedentLabelEncoding(101, 1001);

    // now compute the antecedent occurrences
    std::vector<int> ant_occurrences = enumerate_antecedent(adjacency_lists, antecedent_graph_ids, antecedent_pattern_edges,
                                           antecedent_pattern_edges_ADDED, antecedent_pattern_node_id_ADDED,
                                           antecedent_pattern_node_labels_ADDED,
                                           antecedent_pattern_isolated_node_id,
                                           antecedent_pattern_isolated_node_labels,
                                           antecedent_pattern_isolated_node_changetime, pu, debugging,
                                           search_for_anomalies,
                                           nodes_occupied_by_antecedent, edges_occupied_by_antecedent,
                                           anomaly_patterns);

    ASSERT_EQ(true, false);

}

TEST(DGRSubgraphMiningEnumerateAntecedent, single_graph_partially_separate_antecedent)
{
    // the antecedent is in the same position as pattern and there is also another occurrence, partially overlapping the pattern ocurrence
    ASSERT_EQ(true, false);
}


TEST(DGRSubgraphMiningEnumerateAntecedent, single_graph_separate_antecedent)
{
    // the antecedent is in the same position as pattern and there is also another occurrence not overlapping the pattern ocurrence
    ASSERT_EQ(true, false);
}