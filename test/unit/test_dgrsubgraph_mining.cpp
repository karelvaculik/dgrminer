//
// Created by karbal on 22.2.18.
//


#include "AdjacencyList.h"
#include "DGRSubgraphMining.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::_;
using ::testing::Invoke;
using namespace dgrminer;


TEST(DGRSubgraphMining, test_find_addition_nodes_and_edges)
{
    AdjacencyListCrate adjacency_list;
    std::vector<std::array<int, PAT___SIZE>> antecedent_pattern_edges_ADDED;
    std::map<char, int> from_pattern_id_to_adj_id_map;

    std::vector<int> antecedent_pattern_node_id_ADDED;
    std::vector<int> antecedent_pattern_node_labels_ADDED;
    std::vector<std::array<int, ADJ_NODES___SIZE>> adj_list_nodes;

    antecedent_pattern_node_labels_ADDED.push_back(10);
    antecedent_pattern_node_labels_ADDED.push_back(11);
    antecedent_pattern_node_labels_ADDED.push_back(12);

    // ID, CHANGETIME, LABEL
    std::array<int, 3> node1 = { 0, 0, 10 };
    std::array<int, 3> node2 = { 1, 0, 11 };
    std::array<int, 3> node3 = { 2, 1, 2 };
    std::array<int, 3 > node4 = { 3, 1, 2 };
    std::array<int, 3> node5 = { 4, -1, 12 };
    std::array<int, 3> node6 = { 5, -1, 11 };
    adj_list_nodes.push_back(node1);
    adj_list_nodes.push_back(node2);
    adj_list_nodes.push_back(node3);
    adj_list_nodes.push_back(node4);
    adj_list_nodes.push_back(node5);
    adj_list_nodes.push_back(node6);


    // OUTPUT will be here:
    std::set<non_dummy_vertices_and_edges> sets_of_used_edges;
    std::set<int> nodes_occupied_by_antecedent;
    std::set<int> edges_occupied_by_antecedent;

    find_addition_nodes_and_edges(adjacency_list, antecedent_pattern_edges_ADDED, antecedent_pattern_node_id_ADDED,
                                  antecedent_pattern_node_labels_ADDED, from_pattern_id_to_adj_id_map, adj_list_nodes, sets_of_used_edges, nodes_occupied_by_antecedent, edges_occupied_by_antecedent, true);

    // TODO check the results
}



TEST(DGRSubgraphMining, test_find_addition_nodes_and_edges2)
{
    AdjacencyListCrate adjacency_list;
    std::vector<std::array<int, PAT___SIZE>> antecedent_pattern_edges_ADDED;
    std::map<char, int> from_pattern_id_to_adj_id_map;


    std::vector<int> antecedent_pattern_node_id_ADDED;
    std::vector<int> antecedent_pattern_node_labels_ADDED;
    std::vector<std::array<int, ADJ_NODES___SIZE>> adj_list_nodes;

    // addition stuff:
    antecedent_pattern_node_id_ADDED.push_back(1);
    antecedent_pattern_node_id_ADDED.push_back(2);
    antecedent_pattern_node_id_ADDED.push_back(4);
    antecedent_pattern_node_id_ADDED.push_back(5);

    antecedent_pattern_node_labels_ADDED.push_back(30);
    antecedent_pattern_node_labels_ADDED.push_back(30);
    antecedent_pattern_node_labels_ADDED.push_back(20);
    antecedent_pattern_node_labels_ADDED.push_back(21);

    std::array<int, PAT___SIZE> edge_ADDED_0 = { 0, 1, 10, -5, 0, 100, 0, 30,  0, 0 };
    std::array<int, PAT___SIZE> edge_ADDED_1 = { 1, 2, 30,  0, 0, 100, 0, 30,  0, 1 };
    std::array<int, PAT___SIZE> edge_ADDED_2 = { 2, 3, 30,  0, 0, 100, 0, 10, -5, 2 };
    std::array<int, PAT___SIZE> edge_ADDED_3 = { 0, 4, 10, -5, 0, 100, 0, 20,  0, 3 };
    std::array<int, PAT___SIZE> edge_ADDED_4 = { 4, 5, 20,  0, 0, 100, 0, 21,  0, 4 };
    std::array<int, PAT___SIZE> edge_ADDED_5 = { 5, 3, 21,  0, 0, 100, 0, 10, -5, 5 };
    antecedent_pattern_edges_ADDED.push_back(edge_ADDED_0);
    antecedent_pattern_edges_ADDED.push_back(edge_ADDED_1);
    antecedent_pattern_edges_ADDED.push_back(edge_ADDED_2);
    antecedent_pattern_edges_ADDED.push_back(edge_ADDED_3);
    antecedent_pattern_edges_ADDED.push_back(edge_ADDED_4);
    antecedent_pattern_edges_ADDED.push_back(edge_ADDED_5);

    // adjacency list

    // nodes
    // ID, CHANGETIME, LABEL
    std::array<int, 3> node0 = { 0, -10, 100 };
    std::array<int, 3> node1 = { 1, -10, 100 };
    std::array<int, 3> node2 = { 2, -10, 100 };
    std::array<int, 3> node3 = { 3, -10, 100 };
    std::array<int, 3> node4 = { 4, -10, 100 };
    std::array<int, 3> node5 = { 5, -10, 100 };
    std::array<int, 3> node6 = { 6, -10, 100 };
    std::array<int, 3> node7 = { 7, -10, 100 };
    std::array<int, 3> node8 = { 8, -10, 100 };
    std::array<int, 3> node9 = { 9, -10, 100 };

    std::array<int, 3> node10 = { 10, -5, 10 };
    std::array<int, 3> node11 = { 11, 0, 30 };
    std::array<int, 3> node12 = { 12, 0, 30 };
    std::array<int, 3> node13 = { 13, -5, 10 };
    std::array<int, 3> node14 = { 14, 0, 20 };
    std::array<int, 3> node15 = { 15, 0, 21 };
    std::array<int, 3> node16 = { 16, 0, 20 };
    std::array<int, 3> node17 = { 17, 0, 21 };

    adj_list_nodes.push_back(node0);
    adj_list_nodes.push_back(node1);
    adj_list_nodes.push_back(node2);
    adj_list_nodes.push_back(node3);
    adj_list_nodes.push_back(node4);
    adj_list_nodes.push_back(node5);
    adj_list_nodes.push_back(node6);
    adj_list_nodes.push_back(node7);
    adj_list_nodes.push_back(node8);
    adj_list_nodes.push_back(node9);

    adj_list_nodes.push_back(node10);
    adj_list_nodes.push_back(node11);
    adj_list_nodes.push_back(node12);
    adj_list_nodes.push_back(node13);
    adj_list_nodes.push_back(node14);
    adj_list_nodes.push_back(node15);
    adj_list_nodes.push_back(node16);
    adj_list_nodes.push_back(node17);


    // edges
    std::vector<std::vector<int>> adjacencyList;
    std::vector<int> node_neighbors_0 = {};
    std::vector<int> node_neighbors_1 = {};
    std::vector<int> node_neighbors_2 = {};
    std::vector<int> node_neighbors_3 = {};
    std::vector<int> node_neighbors_4 = {};
    std::vector<int> node_neighbors_5 = {};
    std::vector<int> node_neighbors_6 = {};
    std::vector<int> node_neighbors_7 = {};
    std::vector<int> node_neighbors_8 = {};
    std::vector<int> node_neighbors_9 = {};

    std::vector<int> node_neighbors_10 = { 11, 14 };
    std::vector<int> node_neighbors_11 = { 10, 12 };
    std::vector<int> node_neighbors_12 = { 11, 13 };
    std::vector<int> node_neighbors_13 = { 12, 17 };
    std::vector<int> node_neighbors_14 = { 10, 15 };
    std::vector<int> node_neighbors_15 = { 14 };
    std::vector<int> node_neighbors_16 = { 17 };
    std::vector<int> node_neighbors_17 = { 13, 16 };

    adjacencyList.push_back(node_neighbors_0);
    adjacencyList.push_back(node_neighbors_1);
    adjacencyList.push_back(node_neighbors_2);
    adjacencyList.push_back(node_neighbors_3);
    adjacencyList.push_back(node_neighbors_4);
    adjacencyList.push_back(node_neighbors_5);
    adjacencyList.push_back(node_neighbors_6);
    adjacencyList.push_back(node_neighbors_7);
    adjacencyList.push_back(node_neighbors_8);
    adjacencyList.push_back(node_neighbors_9);
    adjacencyList.push_back(node_neighbors_10);
    adjacencyList.push_back(node_neighbors_11);
    adjacencyList.push_back(node_neighbors_12);
    adjacencyList.push_back(node_neighbors_13);
    adjacencyList.push_back(node_neighbors_14);
    adjacencyList.push_back(node_neighbors_15);
    adjacencyList.push_back(node_neighbors_16);
    adjacencyList.push_back(node_neighbors_17);


    // more info
    std::vector<std::vector<std::array<int, 8>>> adjacencyMoreInfo;

    std::vector<std::array<int, 8>> more_info_0;
    std::vector<std::array<int, 8>> more_info_1;
    std::vector<std::array<int, 8>> more_info_2;
    std::vector<std::array<int, 8>> more_info_3;
    std::vector<std::array<int, 8>> more_info_4;
    std::vector<std::array<int, 8>> more_info_5;
    std::vector<std::array<int, 8>> more_info_6;
    std::vector<std::array<int, 8>> more_info_7;
    std::vector<std::array<int, 8>> more_info_8;
    std::vector<std::array<int, 8>> more_info_9;

    std::array<int, 8> more_info_10_11 = { 10, -5, 0, 100, 0, 30,  0, 11 };
    std::array<int, 8> more_info_10_14 = { 10, -5, 0, 100, 0, 20,  0, 14 };
    std::vector<std::array<int, 8>> more_info_10;
    more_info_10.push_back(more_info_10_11);
    more_info_10.push_back(more_info_10_14);

    std::array<int, 8> more_info_11_10 = { 30, 0, 0, 100, 0, 10,  -5, 11 };
    std::array<int, 8> more_info_11_12 = { 30,  0, 0, 100, 0, 30,  0, 12 };
    std::vector<std::array<int, 8>> more_info_11;
    more_info_11.push_back(more_info_11_10);
    more_info_11.push_back(more_info_11_12);

    std::array<int, 8> more_info_12_11 = { 30,  0, 0, 100, 0, 30,  0, 12 };
    std::array<int, 8> more_info_12_13 = { 30,  0, 0, 100, 0, 10, -5, 13 };
    std::vector<std::array<int, 8>> more_info_12;
    more_info_12.push_back(more_info_12_11);
    more_info_12.push_back(more_info_12_13);

    std::array<int, 8> more_info_13_12 = { 10, -5, 0, 100, 0, 30,  0, 13 };
    std::array<int, 8> more_info_13_17 = { 10, -5, 0, 100, 0, 21,  0, 17 };
    std::vector<std::array<int, 8>> more_info_13;
    more_info_13.push_back(more_info_13_12);
    more_info_13.push_back(more_info_13_17);

    std::array<int, 8> more_info_14_10 = { 20,  0, 0, 100, 0, 10, -5, 14 };
    std::array<int, 8> more_info_14_15 = { 20,  0, 0, 100, 0, 21,  0, 15 };
    std::vector<std::array<int, 8>> more_info_14;
    more_info_14.push_back(more_info_14_10);
    more_info_14.push_back(more_info_14_15);

    std::array<int, 8> more_info_15_14 = { 21,  0, 0, 100, 0, 20, 0, 15 };
    std::vector<std::array<int, 8>> more_info_15;
    more_info_15.push_back(more_info_15_14);

    std::array<int, 8> more_info_16_17 = { 20,  0, 0, 100, 0, 21, 0, 16 };
    std::vector<std::array<int, 8>> more_info_16;
    more_info_16.push_back(more_info_16_17);

    std::array<int, 8> more_info_17_13 = { 21,  0, 0, 100, 0, 10, -5, 17 };
    std::array<int, 8> more_info_17_16 = { 21,  0, 0, 100, 0, 20, 0, 16 };
    std::vector<std::array<int, 8>> more_info_17;
    more_info_17.push_back(more_info_17_13);
    more_info_17.push_back(more_info_17_16);

    adjacencyMoreInfo.push_back(more_info_0);
    adjacencyMoreInfo.push_back(more_info_1);
    adjacencyMoreInfo.push_back(more_info_2);
    adjacencyMoreInfo.push_back(more_info_3);
    adjacencyMoreInfo.push_back(more_info_4);
    adjacencyMoreInfo.push_back(more_info_5);
    adjacencyMoreInfo.push_back(more_info_6);
    adjacencyMoreInfo.push_back(more_info_7);
    adjacencyMoreInfo.push_back(more_info_8);
    adjacencyMoreInfo.push_back(more_info_9);
    adjacencyMoreInfo.push_back(more_info_10);
    adjacencyMoreInfo.push_back(more_info_11);
    adjacencyMoreInfo.push_back(more_info_12);
    adjacencyMoreInfo.push_back(more_info_13);
    adjacencyMoreInfo.push_back(more_info_14);
    adjacencyMoreInfo.push_back(more_info_15);
    adjacencyMoreInfo.push_back(more_info_16);
    adjacencyMoreInfo.push_back(more_info_17);

    adjacency_list.adjacencyList = adjacencyList;
    adjacency_list.adjacencyEdgeInfo = adjacencyMoreInfo;
    adjacency_list.nodes = adj_list_nodes;

    // already mapped nodes (nonaddition):
    from_pattern_id_to_adj_id_map[0] = 10;
    from_pattern_id_to_adj_id_map[3] = 13;

    // OUTPUT will be here:
    std::set<non_dummy_vertices_and_edges> sets_of_used_edges;
    std::set<int> nodes_occupied_by_antecedent;
    std::set<int> edges_occupied_by_antecedent;

    find_addition_nodes_and_edges(adjacency_list, antecedent_pattern_edges_ADDED, antecedent_pattern_node_id_ADDED,
                                  antecedent_pattern_node_labels_ADDED, from_pattern_id_to_adj_id_map, adj_list_nodes, sets_of_used_edges, nodes_occupied_by_antecedent, edges_occupied_by_antecedent, true);

    cout << "RESULTS: " << endl;
    for (set< non_dummy_vertices_and_edges >::iterator new_it = sets_of_used_edges.begin(); new_it != sets_of_used_edges.end(); new_it++)
    {
        for (set< int >::iterator it = new_it->non_dummy_edges.begin(); it != new_it->non_dummy_edges.end(); ++it)
        {
            cout << *it << ", ";
        }
        cout << endl;
    }

    cout << "MAXIMAL RESULTS: " << endl;
    std::vector<non_dummy_vertices_and_edges> vector_of_edge_sets;
    for (set< non_dummy_vertices_and_edges >::iterator new_it = sets_of_used_edges.begin(); new_it != sets_of_used_edges.end(); new_it++)
    {
        vector_of_edge_sets.push_back(*new_it);
    }
    std::vector<non_dummy_vertices_and_edges> maximal_patterns_of_addition_edges = find_maximal_subsets_of_ndve(vector_of_edge_sets);

    for (size_t i = 0; i < maximal_patterns_of_addition_edges.size(); i++)
    {
        for (set< int >::iterator new_it = maximal_patterns_of_addition_edges[i].non_dummy_edges.begin(); new_it != maximal_patterns_of_addition_edges[i].non_dummy_edges.end(); new_it++)
        {
            cout << *new_it << ", ";
        }
        cout << endl;
    }

    // TODO: check results


}