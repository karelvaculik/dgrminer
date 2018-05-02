//
// Created by karel on 18.2.18.
//

#ifndef DGRMINER_DGRSUBGRAPHMINING_H
#define DGRMINER_DGRSUBGRAPHMINING_H


#include "AdjacencyList.h"
#include "PartialUnion.h"
#include "HelperFunctions.h"

#include <limits>
#include <vector>
#include <unordered_map>

namespace dgrminer
{
    struct children_candidate
    {
        std::array<int, 10> elements;
        mutable std::set<int> occurrences;
        mutable std::unordered_map<int, std::set<std::set<int>>> multiple_occurrences;
    };

    struct node_label_changetime
    {
        int label;
        int changetime;
    };

    inline bool operator<(const children_candidate& lhs, const children_candidate& rhs)
    {
        // (ignoring edge id)
        return is_pattern_edge_smaller_than(lhs.elements, rhs.elements, true);
    }
    inline bool operator==(const children_candidate& lhs, const children_candidate& rhs)
    {
        // (ignoring edge id)
        return is_pattern_edge_equal_to(lhs.elements, rhs.elements, true);
    }



    std::set<children_candidate> enumerate(std::vector<AdjacencyListCrate> &adjacency_lists, std::vector<int> &graph_ids, std::vector<std::array<int, PAT___SIZE>> &edge_list,
                                           std::vector<std::set<int>> &nodes_occupied_by_antecedent, std::vector<std::set<int>> &edges_occupied_by_antecedent, bool new_measures,
                                           std::unordered_map<int, std::set<std::set<int>>> &multiple_occurrences
    );
    std::vector<std::array<int, 8>> find_forward_edge_candidates(std::vector<std::vector<int>> &adj_list,
                                                                 std::vector<std::vector<std::array<int, 8>>> &adj_more_info,
                                                                 int src, std::vector<int> &sequence_of_nodes, std::vector<int> &real_second_edge_ids);
    std::vector<std::array<int, 8>> find_backward_edge_candidates(std::vector<std::vector<int>> &adj_list,
                                                                  std::vector<std::vector<std::array<int, 8>>> &adj_more_info, int src, int dst,
                                                                  std::vector<int> &real_ids_src, std::vector<int> &real_ids_dst, std::vector<int> &real_ids_dst_ind);

    void save_pattern(std::vector<std::array<int, PAT___SIZE>> &edge_list, std::vector<int> &graph_ids,
                      results_crate * results, bool set_of_graphs, PartialUnion pu, int pattern_support_absolute,
                      double support_relative, std::set<int> &antecedent_occurrences, double confidence,
                      bool verbose);
    void save_pattern_anomaly(std::vector<std::array<int, PAT___SIZE>> &edge_list, std::set<int> &graph_ids,
                              results_crate_anomalies * results_anomaly, bool set_of_graphs, PartialUnion pu,
                              int anomaly_id_of_explanation_pattern, double anomaly_outlierness, bool verbose);


    /**
     * This function removes added nodes and edges, and returns antecedent part from the rest
     * @param edge_list edge list of the pattern
     * @param antecedent_pattern_edges list of antecedent edges (output param)
     * @param isolated_nodes_label list of labels of isolated nodes (output param)
     * @param isolated_nodes_changetime list of changetimes of isolated nodes (output param)
     * @param isolated_nodes_id list of ids of isolated nodes (output param)
     * @param antecedent_pattern_edges_ADDED list of pattern edges that represent addition (output param)
     * @param antecedent_pattern_isolated_node_id_ADDED list of ids of pattern nodes that represent addition (output param)
     * @param antecedent_pattern_isolated_node_labels_ADDED list of labels of pattern nodes that represent addition (output param)
     */
    void compute_antecedent_pattern_from_pattern(std::vector<std::array<int, PAT___SIZE>> &edge_list,
                                                 std::vector<std::array<int, PAT___SIZE>> &antecedent_pattern_edges,
                                                 std::vector<int> &isolated_nodes_label,
                                                 std::vector<int> &isolated_nodes_changetime,
                                                 std::vector<int> &isolated_nodes_id,
                                                 std::vector<std::array<int, PAT___SIZE>> &antecedent_pattern_edges_ADDED,
                                                 std::vector<int> &antecedent_pattern_isolated_node_id_ADDED,
                                                 std::vector<int> &antecedent_pattern_isolated_node_labels_ADDED);

    void find_addition_edges(AdjacencyListCrate & adjacency_list, std::vector<std::array<int, PAT___SIZE>> & antecedent_pattern_edges_ADDED,
                             std::map<char, int> & from_pattern_id_to_adj_id_map, int num_of_nondummy_nodes, std::vector<std::set<int_pair>> & edge_mapping_sets,
                             std::set<int> & edges_occupied_by_antecedent, bool debug);

    void find_addition_nodes_and_edges(AdjacencyListCrate & adjacency_list, std::vector<std::array<int, PAT___SIZE>> & antecedent_pattern_edges_ADDED,
                                       std::vector<int> & antecedent_pattern_node_id_ADDED,
                                       std::vector<int> & antecedent_pattern_node_labels_ADDED,
                                       std::map<char, int> & from_pattern_id_to_adj_id_map,
                                       std::vector<std::array<int, ADJ_NODES___SIZE>> & adj_list_nodes,
                                       std::set<non_dummy_vertices_and_edges> & sets_of_used_edges,
                                       std::set<int> & nodes_occupied_by_antecedent, std::set<int> & edges_occupied_by_antecedent,
                                       bool debug);

    void find_isolated_nodes_and_addition_edges(AdjacencyListCrate & adjacency_list, std::vector<std::array<int, PAT___SIZE>> & antecedent_pattern_edges_ADDED,
                                                std::vector<int> & antecedent_pattern_isolated_node_id_ADDED,
                                                std::vector<int> & antecedent_pattern_isolated_node_labels_ADDED,
                                                std::vector<int> & antecedent_pattern_isolated_node_id,
                                                std::vector<int> & antecedent_pattern_isolated_node_labels,
                                                std::vector<int> & antecedent_pattern_isolated_node_changetime, PartialUnion pu, bool search_for_anomalies,
                                                std::set<int> & nodes_occupied_by_antecedent, std::set<int> & edges_occupied_by_antecedent,
                                                std::set<int> & occupied_node_ids_in_adj_list,
                                                int g_id,
                                                std::vector<std::array<int, PAT___SIZE>> & anomaly_pattern_edges,
                                                std::set<anomaly_pattern_with_occurrences> & anomaly_patterns,
                                                bool debug);

    std::vector<int> enumerate_antecedent(std::vector<AdjacencyListCrate> &adjacency_lists,
                                          std::vector<int> &graph_ids,
                                          std::vector<std::array<int, PAT___SIZE>> &antecedent_pattern_edges,
                                          std::vector<std::array<int, PAT___SIZE>> &antecedent_pattern_edges_ADDED,
                                          std::vector<int> &antecedent_pattern_isolated_node_id_ADDED,
                                          std::vector<int> &antecedent_pattern_isolated_node_labels_ADDED,
                                          std::vector<int> &antecedent_pattern_isolated_node_id,
                                          std::vector<int> &antecedent_pattern_isolated_node_labels,
                                          std::vector<int> &antecedent_pattern_isolated_node_changetime,
                                          PartialUnion pu, bool debugging, bool search_for_anomalies,
                                          std::vector<std::set<int>> &nodes_occupied_by_antecedent,
                                          std::vector<std::set<int>> &edges_occupied_by_antecedent,
                                          std::set<anomaly_pattern_with_occurrences> &anomaly_patterns);

    void DGRSubgraphMining(std::vector<AdjacencyListCrate> &adjacency_lists, std::vector<int> & graph_ids,
                           std::vector<std::array<int, 10>> &pattern_edge_list, int support_as_absolute, std::vector<std::array<int, 8>> &starting_edges, results_crate * results, results_crate_anomalies * results_anomalies,
                           int max_absolute_support, double min_confidence, bool compute_confidence, PartialUnion pu, std::vector<int> & antecedent_graph_ids, bool set_of_graphs, bool search_for_anomalies,
                           double min_anomaly_outlierness,
                           std::string output_file, bool verbose, bool new_measures);


	template<typename T>
    inline size_t support(const T &occurrences,
						  const std::unordered_map<int, std::set<std::set<int>>> &multiple_occurrences,
						  bool new_measures
	);

}


#endif //DGRMINER_DGRSUBGRAPHMINING_H
