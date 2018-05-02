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


TEST(FileReader, read_intro_example) {
    FileReader fr;
    PartialUnion pu;
    bool search_for_anomalies = false;
    int window_size = 10;
    std::string input_file = "data/intro_example";

    pu = fr.read_dynamic_graph_from_file(input_file, window_size, search_for_anomalies);

    ASSERT_EQ(pu.getNumberOfNodes(), 23);
    ASSERT_EQ(pu.getNumberOfSnapshots(), 5);

    std::vector<std::array<int, 8>> edges_ground_truth = {
            {1, 1, 2, 6,  1, 0, 0, 1},
            {2, 1, 2, 11, 1, 1, 1, 1},
            {2, 2, 3, 6,  1, 0, 0, 2},
            {2, 2, 4, 6,  1, 0, 0, 3},
            {3, 2, 3, 11, 1, 1, 1, 2},
            {3, 2, 4, 11, 1, 1, 1, 3},
            {3, 3, 1, 6,  1, 0, 0, 4},
            {3, 4, 1, 6,  1, 0, 0, 5},
            {3, 1, 5, 6,  1, 0, 0, 6},
            {4, 3, 1, 11, 1, 1, 1, 4},
            {4, 4, 1, 11, 1, 1, 1, 5},
            {4, 1, 5, 11, 1, 1, 1, 6},
            {4, 5, 3, 6,  1, 0, 0, 7},
            {4, 5, 4, 6,  1, 0, 0, 8},
            {5, 5, 3, 11, 1, 1, 1, 7},
            {5, 5, 4, 11, 1, 1, 1, 8},
            {5, 3, 1, 6,  1, 0, 0, 9},
            {5, 4, 1, 6,  1, 0, 0, 10}
    };
//    println(pu.getEdges());
//    println(pu.getEncodingLabel(6));
//    println(pu.getEncodingLabel(11));

    ASSERT_EQ(pu.getEdges(), edges_ground_truth);
}
