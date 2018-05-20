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


TEST(DGRSubgraphMiningEnumerate, single_graph_single_occurrence)
{
    std::vector<AdjacencyListCrate> adjacency_lists;
    std::vector<int> graph_ids;
    std::vector<std::set<int>> nodes_occupied_by_antecedent;
    std::vector<std::set<int>> edges_occupied_by_antecedent;


    std::vector<std::vector<int>> adjacencyList = {
            {1, 2},
            {0, 2},
            {0, 1}
    };
    std::array<int, ADJ_INFO___SIZE> e_info0 = {10,  -5, 0, 100, 0, 20, -10, 0}; // = flipAdjacencyInfo(newedges[e_index]
    std::array<int, ADJ_INFO___SIZE> e_info1 = {20, -10, 0, 101, 0, 30, -15, 1};
    std::array<int, ADJ_INFO___SIZE> e_info2 = {30, -15, 0, 102, 0, 10,  -5, 2};
    std::vector<std::vector<std::array<int, ADJ_INFO___SIZE>>> adjacencyEdgeInfo = {
            {e_info0, flipAdjacencyInfo(e_info2)},
            {flipAdjacencyInfo(e_info0), e_info1},
            {e_info2, flipAdjacencyInfo(e_info1)}
    };
    std::vector<std::array<int, ADJ_NODES___SIZE>> adjacencyListNodes = {
            {0,  -5, 10},
            {1, -10, 20},
            {2, -15, 30}
    };
    AdjacencyListCrate alc = AdjacencyListCrate();
    alc.adjacencyEdgeInfo = adjacencyEdgeInfo;
    alc.adjacencyList = adjacencyList;
    alc.nodes = adjacencyListNodes;
    adjacency_lists.push_back(alc);
    graph_ids.push_back(0);

    std::vector<std::array<int, PAT___SIZE>> pattern_edge_list = {
            {0, 1, 20, -10, 0, 101, 0, 30, -15, 0}
    };
    bool new_measures = false;
    std::unordered_map<int, std::set<std::set<int>>> multiple_occurrences;
    std::unordered_map<int, std::set<std::set<int>>> multiple_antecedent_occurrences;

    std::set<children_candidate> children = enumerate(adjacency_lists, graph_ids, pattern_edge_list,
                                                      nodes_occupied_by_antecedent, edges_occupied_by_antecedent,
                                                      new_measures, multiple_occurrences,
                                                      multiple_antecedent_occurrences);

    std::array<int, 10> elements_1 = {1, 2, 30, -15, 0, 102, 0, 10, -5, 2};
    std::set<int> occurrences_1 = {0};
    children_candidate children_candidate_1;
    children_candidate_1.elements = elements_1;
    children_candidate_1.occurrences.insert(0);
    children_candidate children_candidate_2;
    std::array<int, 10> elements_2 = {0, 2, 20, -10, 0, 100, 0, 10, -5, 0 };
    std::set<int> occurrences_2 = {0};
    children_candidate_2.elements = elements_2;
    children_candidate_2.occurrences.insert(0);

    std::set<children_candidate> children_ground_truth;
    children_ground_truth.insert(children_candidate_1);
    children_ground_truth.insert(children_candidate_2);

    ASSERT_EQ(children, children_ground_truth);

    std::vector<std::set<int>> nodes_occupied_by_antecedent_ground_truth = {{1, 2}};
    std::vector<std::set<int>> edges_occupied_by_antecedent_ground_truth = {{1}};

    ASSERT_EQ(nodes_occupied_by_antecedent, nodes_occupied_by_antecedent_ground_truth);
    ASSERT_EQ(edges_occupied_by_antecedent, edges_occupied_by_antecedent_ground_truth);
}


TEST(DGRSubgraphMiningEnumerate, single_graph_single_occurrence_with_new_measures)
{
    std::vector<AdjacencyListCrate> adjacency_lists;
    std::vector<int> graph_ids;
    std::vector<std::set<int>> nodes_occupied_by_antecedent;
    std::vector<std::set<int>> edges_occupied_by_antecedent;


    std::vector<std::vector<int>> adjacencyList = {
            {1, 2},
            {0, 2},
            {0, 1}
    };
    std::array<int, ADJ_INFO___SIZE> e_info0 = {10,  -5, 0, 100, 0, 20, -10, 0}; // = flipAdjacencyInfo(newedges[e_index]
    std::array<int, ADJ_INFO___SIZE> e_info1 = {20, -10, 0, 101, 0, 30, -15, 1};
    std::array<int, ADJ_INFO___SIZE> e_info2 = {30, -15, 0, 102, 0, 10,  -5, 2};
    std::vector<std::vector<std::array<int, ADJ_INFO___SIZE>>> adjacencyEdgeInfo = {
            {e_info0, flipAdjacencyInfo(e_info2)},
            {flipAdjacencyInfo(e_info0), e_info1},
            {e_info2, flipAdjacencyInfo(e_info1)}
    };
    std::vector<std::array<int, ADJ_NODES___SIZE>> adjacencyListNodes = {
            {0,  -5, 10},
            {1, -10, 20},
            {2, -15, 30}
    };
    AdjacencyListCrate alc = AdjacencyListCrate();
    alc.adjacencyEdgeInfo = adjacencyEdgeInfo;
    alc.adjacencyList = adjacencyList;
    alc.nodes = adjacencyListNodes;
    adjacency_lists.push_back(alc);
    graph_ids.push_back(0);

    std::vector<std::array<int, PAT___SIZE>> pattern_edge_list = {
            {0, 1, 20, -10, 0, 101, 0, 30, -15, 0}
    };
    bool new_measures = true;
    std::unordered_map<int, std::set<std::set<int>>> multiple_occurrences;
    std::unordered_map<int, std::set<std::set<int>>> multiple_antecedent_occurrences;

    std::set<children_candidate> children = enumerate(adjacency_lists, graph_ids, pattern_edge_list,
                                                      nodes_occupied_by_antecedent, edges_occupied_by_antecedent,
                                                      new_measures, multiple_occurrences,
                                                      multiple_antecedent_occurrences);

    std::array<int, 10> elements_1 = {1, 2, 30, -15, 0, 102, 0, 10, -5, 2};
    std::set<int> occurrences_1 = {0};
    children_candidate children_candidate_1;
    children_candidate_1.elements = elements_1;
    children_candidate_1.occurrences.insert(0);
    children_candidate children_candidate_2;
    std::array<int, 10> elements_2 = {0, 2, 20, -10, 0, 100, 0, 10, -5, 0 };
    std::set<int> occurrences_2 = {0};
    children_candidate_2.elements = elements_2;
    children_candidate_2.occurrences.insert(0);

    std::set<children_candidate> children_ground_truth;
    children_ground_truth.insert(children_candidate_1);
    children_ground_truth.insert(children_candidate_2);

    ASSERT_EQ(children, children_ground_truth);

    std::vector<std::set<int>> nodes_occupied_by_antecedent_ground_truth = {{1, 2}};
    std::vector<std::set<int>> edges_occupied_by_antecedent_ground_truth = {{1}};

    ASSERT_EQ(nodes_occupied_by_antecedent, nodes_occupied_by_antecedent_ground_truth);
    ASSERT_EQ(edges_occupied_by_antecedent, edges_occupied_by_antecedent_ground_truth);

    std::set<std::set<int>> occurrence_nodes = {{1, 2}};
    std::unordered_map<int, std::set<std::set<int>>> multiple_occurrences_ground_truth;
    multiple_occurrences_ground_truth[0] = occurrence_nodes;
    ASSERT_EQ(multiple_occurrences, multiple_occurrences_ground_truth);
}



TEST(DGRSubgraphMiningEnumerate, single_graph_two_occurrences_with_new_measures)
{
    std::vector<AdjacencyListCrate> adjacency_lists;
    std::vector<int> graph_ids;
    std::vector<std::set<int>> nodes_occupied_by_antecedent;
    std::vector<std::set<int>> edges_occupied_by_antecedent;


    std::vector<std::vector<int>> adjacencyList = {
            {1, 2},
            {0, 2},
            {0, 1}
    };
    std::array<int, ADJ_INFO___SIZE> e_info0 = {20, -10, 0, 100, 0, 20, -10, 0}; // = flipAdjacencyInfo(newedges[e_index]
    std::array<int, ADJ_INFO___SIZE> e_info1 = {20, -10, 0, 101, 0, 30, -15, 1};
    std::array<int, ADJ_INFO___SIZE> e_info2 = {30, -15, 0, 101, 0, 20, -10, 2};
    std::vector<std::vector<std::array<int, ADJ_INFO___SIZE>>> adjacencyEdgeInfo = {
            {e_info0, flipAdjacencyInfo(e_info2)},
            {flipAdjacencyInfo(e_info0), e_info1},
            {e_info2, flipAdjacencyInfo(e_info1)}
    };
    std::vector<std::array<int, ADJ_NODES___SIZE>> adjacencyListNodes = {
            {0, -10, 20},
            {1, -10, 20},
            {2, -15, 30}
    };
    AdjacencyListCrate alc = AdjacencyListCrate();
    alc.adjacencyEdgeInfo = adjacencyEdgeInfo;
    alc.adjacencyList = adjacencyList;
    alc.nodes = adjacencyListNodes;
    adjacency_lists.push_back(alc);
    graph_ids.push_back(0);

    std::vector<std::array<int, PAT___SIZE>> pattern_edge_list = {
            {0, 1, 20, -10, 0, 101, 0, 30, -15, 0}
    };
    bool new_measures = true;
    std::unordered_map<int, std::set<std::set<int>>> multiple_occurrences;
    std::unordered_map<int, std::set<std::set<int>>> multiple_antecedent_occurrences;

    std::set<children_candidate> children = enumerate(adjacency_lists, graph_ids, pattern_edge_list,
                                                      nodes_occupied_by_antecedent, edges_occupied_by_antecedent,
                                                      new_measures, multiple_occurrences,
                                                      multiple_antecedent_occurrences);

    std::array<int, 10> elements_1 = {1, 2, 30, -15, 0, 101, 0, 20, -10, 1};
    std::set<int> occurrences_1 = {0};
    children_candidate children_candidate_1;
    children_candidate_1.elements = elements_1;
    children_candidate_1.occurrences.insert(0);
    children_candidate children_candidate_2;
    std::array<int, 10> elements_2 = {0, 2, 20, -10, 0, 100, 0, 20, -10, 0 };
    std::set<int> occurrences_2 = {0};
    children_candidate_2.elements = elements_2;
    children_candidate_2.occurrences.insert(0);

    std::set<children_candidate> children_ground_truth;
    children_ground_truth.insert(children_candidate_1);
    children_ground_truth.insert(children_candidate_2);

    ASSERT_EQ(children, children_ground_truth);

    std::vector<std::set<int>> nodes_occupied_by_antecedent_ground_truth = {{0, 1, 2}};
    std::vector<std::set<int>> edges_occupied_by_antecedent_ground_truth = {{1, 2}};

    ASSERT_EQ(nodes_occupied_by_antecedent, nodes_occupied_by_antecedent_ground_truth);
    ASSERT_EQ(edges_occupied_by_antecedent, edges_occupied_by_antecedent_ground_truth);

    std::set<std::set<int>> occurrence_nodes = {{0, 2}, {1, 2}};
    std::unordered_map<int, std::set<std::set<int>>> multiple_occurrences_ground_truth;
    multiple_occurrences_ground_truth[0] = occurrence_nodes;
    ASSERT_EQ(multiple_occurrences, multiple_occurrences_ground_truth);
}