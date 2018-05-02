//
// Created by karbal on 25.2.18.
//



#include "AdjacencyList.h"
#include "FileReader.h"
#include "PartialUnion.h"
#include "HelperPrintFunctions.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vector>

using ::testing::_;
using ::testing::Invoke;
using namespace dgrminer;


TEST(PartialUnion, create_adjacency_lists) {
    // assumes working file reader

    FileReader fr;
    PartialUnion pu;
    bool search_for_anomalies = false;
    int window_size = 10;
    std::string input_file = "data/intro_example";

    pu = fr.read_dynamic_graph_from_file(input_file, window_size, search_for_anomalies);

    std::set<labeled_node_time> labeled_nodes = pu.compute_labeled_nodes();
    int snapshots = pu.getNumberOfSnapshots();
    std::vector<std::array<int, 8>> newedges;
    std::set<labeled_edge_with_occurrences> edges_set;
    pu.compute_labeled_edges(newedges, edges_set, labeled_nodes, false);
    std::vector<AdjacencyListCrate> adjacency_lists = pu.createAdjacencyLists(newedges, snapshots);

//    for (int i = 0; i < adjacency_lists.size(); ++i) {
//        println("LIST: ");
//        println(adjacency_lists[i].adjacencyList);
//        println(adjacency_lists[i].adjacencyEdgeInfo);
//    }
//
//    for (int j = 0; j < 14; ++j) {
//        println("LABEL: ", j, " ", pu.getEncodingLabel(j));
//    }

    std::vector<std::vector<int>> adjacency_list_01_ground_truth = {{1},
                                                                    {0},
                                                                    {},
                                                                    {}};
    std::vector<std::vector<std::array<int, ADJ_INFO___SIZE>>> adjacency_info_01_ground_truth = {
            {{2, 0, 1, 6, 0, 4, 0, 1}}, // added nodes with edges (all changetimes = 0)
            {{4, 0, 2, 6, 0, 2, 0, 1}},
            {},
            {}
    };
    std::vector<std::vector<int>> adjacency_list_02_ground_truth = {{1},
                                                                    {0, 2, 3},
                                                                    {1},
                                                                    {1}};
    std::vector<std::vector<std::array<int, ADJ_INFO___SIZE>>> adjacency_info_02_ground_truth = {
            {{8,  1,  1, 11, 1, 3, -1, 1}}, // label change of node with 0 (label 2->8, i.e. +C -> C=>D, changetime 1), label of 1 stays (4 -> 3, i.e. +A -> A, changetime - 1)
            {{3,  -1, 2, 11, 1, 8, 1,  1}, {3, -1, 1, 6, 0, 10, 1, 2}, {3, -1, 1, 6, 0, 10, 1, 3}},
            {{10, 1,  2, 6,  0, 3, -1, 2}},
            {{10, 1,  2, 6,  0, 3, -1, 3}}
    };
    std::vector<std::vector<int>> adjacency_list_03_ground_truth = {{2, 3, 4},
                                                                    {2, 3},
                                                                    {1, 0},
                                                                    {1, 0},
                                                                    {0}};
    std::vector<std::vector<std::array<int, ADJ_INFO___SIZE>>> adjacency_info_03_ground_truth = {
            {{12, 1,  2, 6,  0, 13, 1,  4}, {12, 1,  2, 6,  0, 13, 1, 5}, {12, 1, 1, 6, 0, 4, 0, 6}},
            {{3,  -2, 1, 11, 1, 13, 1,  2}, {3,  -2, 1, 11, 1, 13, 1, 3}},
            {{13, 1,  2, 11, 1, 3,  -2, 2}, {13, 1,  1, 6,  0, 12, 1, 4}},
            {{13, 1,  2, 11, 1, 3,  -2, 3}, {13, 1,  1, 6,  0, 12, 1, 5}},
            {{4,  0,  2, 6,  0, 12, 1,  6}}

    };
    std::vector<std::vector<int>> adjacency_list_04_ground_truth = {{2, 3, 4},
                                                                    {},
                                                                    {0, 4},
                                                                    {0, 4},
                                                                    {0, 2, 3}};
    std::vector<std::vector<std::array<int, ADJ_INFO___SIZE>>> adjacency_info_04_ground_truth = {
            {{8,  1,  2, 11, 1, 10, 1, 4}, {8,  1,  2, 11, 1, 10, 1,  5}, {8, 1,  1, 11, 1, 3,  -1, 6}},
            {},
            {{10, 1,  1, 11, 1, 8,  1, 4}, {10, 1,  2, 6,  0, 3,  -1, 7}},
            {{10, 1,  1, 11, 1, 8,  1, 5}, {10, 1,  2, 6,  0, 3,  -1, 8}},
            {{3,  -1, 2, 11, 1, 8,  1, 6}, {3,  -1, 1, 6,  0, 10, 1,  7}, {3, -1, 1, 6,  0, 10, 1,  8}}

    };
    std::vector<std::vector<int>> adjacency_list_05_ground_truth = {{2, 3},
                                                                    {},
                                                                    {4, 0},
                                                                    {4, 0},
                                                                    {2, 3}};
    std::vector<std::vector<std::array<int, ADJ_INFO___SIZE>>> adjacency_info_05_ground_truth = {
            {{12, 1,  2, 6,  0, 13, 1,  9}, {12, 1,  2, 6,  0, 13, 1, 10}},
            {},
            {{13, 1,  2, 11, 1, 3,  -2, 7}, {13, 1,  1, 6,  0, 12, 1, 9}},
            {{13, 1,  2, 11, 1, 3,  -2, 8}, {13, 1,  1, 6,  0, 12, 1, 10}},
            {{3,  -2, 1, 11, 1, 13, 1,  7}, {3,  -2, 1, 11, 1, 13, 1, 8}}
    };

    ASSERT_EQ(adjacency_lists[0].adjacencyList, adjacency_list_01_ground_truth);
    ASSERT_EQ(adjacency_lists[0].adjacencyEdgeInfo, adjacency_info_01_ground_truth);
    ASSERT_EQ(adjacency_lists[1].adjacencyList, adjacency_list_02_ground_truth);
    ASSERT_EQ(adjacency_lists[1].adjacencyEdgeInfo, adjacency_info_02_ground_truth);
    ASSERT_EQ(adjacency_lists[2].adjacencyList, adjacency_list_03_ground_truth);
    ASSERT_EQ(adjacency_lists[2].adjacencyEdgeInfo, adjacency_info_03_ground_truth);
    ASSERT_EQ(adjacency_lists[3].adjacencyList, adjacency_list_04_ground_truth);
    ASSERT_EQ(adjacency_lists[3].adjacencyEdgeInfo, adjacency_info_04_ground_truth);
    ASSERT_EQ(adjacency_lists[4].adjacencyList, adjacency_list_05_ground_truth);
    ASSERT_EQ(adjacency_lists[4].adjacencyEdgeInfo, adjacency_info_05_ground_truth);

}
