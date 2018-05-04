
#include <vector>
#include <set>
#include <iostream>
#include <algorithm>
#include "AdjacencyList.h"
#include "HelperFunctions.h"
#include "HelperPrintFunctions.h"
#include "DGRSubgraphMining.h"
#include "PartialUnion.h"

namespace dgrminer
{

	using namespace std;

	template<typename T>
	inline size_t support(const T &occurrences,
					  const std::unordered_map<int, std::set<std::set<int>>> &multiple_occurrences,
					  bool new_measures
	) {
	  	if (new_measures) {
		  	size_t support = 0;

			for (auto const &snapshotId : occurrences) {
		  		OverlapGraph og;
		  		std::map<std::set<int>, int> mapping;

		  		for (auto const &occurrence : multiple_occurrences.at(snapshotId)) {
					mapping[occurrence] = og.addVertex();
		  		}

			  	for (
			  		auto firstOccurrence = multiple_occurrences.at(snapshotId).begin();
			  		firstOccurrence != multiple_occurrences.at(snapshotId).end();
			  		++firstOccurrence
				) {
					for (
						auto secondOccurrence = std::next(firstOccurrence);
						secondOccurrence != multiple_occurrences.at(snapshotId).end();
						++secondOccurrence
					) {
						std::set<int> intersect;
						std::set_intersection(
							firstOccurrence->begin(), firstOccurrence->end(),
							secondOccurrence->begin(), secondOccurrence->end(),
							std::inserter(intersect,intersect.begin())
						);

						if (!intersect.empty()) {
						  	og.addEdge(mapping.at(*firstOccurrence), mapping.at(*secondOccurrence));
						}
					}
				}

			  	support += og.computeSupport();
			}

		  	return support;
		}

	  	return occurrences.size();
	}

	// graph_ids: first graph has id == 0
	void DGRSubgraphMining(std::vector<AdjacencyListCrate> &adjacency_lists, std::vector<int> & graph_ids,
                           std::vector<std::array<int, 10>> &pattern_edge_list, int support_as_absolute,
                           std::vector<std::array<int, 8>> &starting_edges, results_crate * results,
                           results_crate_anomalies * results_anomalies,
                           int max_absolute_support, double min_confidence, bool compute_confidence,
                           PartialUnion pu, std::vector<int> &antecedent_graph_ids,
                           bool set_of_graphs, bool search_for_anomalies, double min_anomaly_outlierness,
                           std::string output_file, bool verbose, bool new_measures)
	{
		// check min code
		if (!is_min_code(pattern_edge_list, starting_edges)) {
			// if the code is not minimal, backtrack
			return;
		}

		// here we save for each snapshot node (edge) ids that are occupied by the antecedent occurrences:
		std::vector<std::set<int>> nodes_occupied_by_antecedent;
		std::vector<std::set<int>> edges_occupied_by_antecedent;

	  	std::unordered_map<int, std::set<std::set<int>>> multiple_pattern_occurrences;
	  	std::unordered_map<int, std::set<std::set<int>>> multiple_antecedent_occurrences;

		// find all children of the current pattern
		std::set<children_candidate> children = enumerate(adjacency_lists, graph_ids, pattern_edge_list,
                                                          nodes_occupied_by_antecedent, edges_occupied_by_antecedent, new_measures,
														  multiple_pattern_occurrences, multiple_antecedent_occurrences);

		// remove infrequent children:
		for (auto itr = children.begin(); itr != children.end();) {
			if (set_of_graphs) {
				std::set<int> mapped_occurrences;
				std::set<int>::iterator it2;
				for (it2 = (*itr).occurrences.begin(); it2 != (*itr).occurrences.end(); ++it2) {
					mapped_occurrences.insert(pu.queryMappingSnapshotsToGraphs(*it2));

				}

			  	if ((!new_measures && (mapped_occurrences.size() < support_as_absolute)) ||
					(new_measures && (support<std::set<int>>(
						(*itr).occurrences,
						(*itr).multiple_occurrences,
						new_measures
					) < support_as_absolute))) {
				  	children.erase(itr++);
				}

				else ++itr;
			}
			else {
				if (support<std::set<int>>((*itr).occurrences, (*itr).multiple_occurrences, new_measures) < support_as_absolute) {
				  	children.erase(itr++);
				}

				else ++itr;
			}
		}

		// compute absolute support of the pattern:
	  	int pattern_support_absolute = 0;


		if (set_of_graphs)
		{
			std::set<int> mapped_set_of_graphs;
			for (int id : graph_ids)
			{
				mapped_set_of_graphs.insert(pu.queryMappingSnapshotsToGraphs(id));
			}

		  	pattern_support_absolute = (int) support<std::vector<int>>(graph_ids, multiple_pattern_occurrences, new_measures);
		} else {
		  	pattern_support_absolute = (int) support<std::vector<int>>(graph_ids, multiple_pattern_occurrences, new_measures);
		}

		std::vector<int> ant_occurrences;
		// if computing confidence (and possibly also anomalies)
		if (compute_confidence)
		{
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

			int antecedent_abs_sup = max_absolute_support; // if the antecedent is empty, then its support is number of snapshots / graphs: (only addition changes)
			std::set<int> ant_times;
			std::set<anomaly_pattern_with_occurrences> anomaly_patterns;
			if (antecedent_pattern_edges.size() != 0 || antecedent_pattern_isolated_node_id.size() != 0
				|| antecedent_pattern_edges_ADDED.size() != 0 || antecedent_pattern_node_id_ADDED.size() != 0)
			{
				bool debugging = false;

				ant_occurrences = enumerate_antecedent(adjacency_lists, antecedent_graph_ids, antecedent_pattern_edges,
													   antecedent_pattern_edges_ADDED, antecedent_pattern_node_id_ADDED,
													   antecedent_pattern_node_labels_ADDED,
													   antecedent_pattern_isolated_node_id,
													   antecedent_pattern_isolated_node_labels,
													   antecedent_pattern_isolated_node_changetime, pu, debugging,
													   search_for_anomalies,
													   nodes_occupied_by_antecedent, edges_occupied_by_antecedent,
													   anomaly_patterns,
													   multiple_antecedent_occurrences,
													   new_measures);

				// if there are some non-addition elements, compute antecedent support from the occurrences
				// (otherwise we use number of snapshots)
				if (antecedent_pattern_edges.size() != 0 || antecedent_pattern_isolated_node_id.size() != 0)
				{

					for (size_t i = 0; i < graph_ids.size(); i++) {
						if (set_of_graphs && !new_measures)
						{
							ant_times.insert(pu.queryMappingSnapshotsToGraphs(graph_ids[i]));
						}
						else
						{
							ant_times.insert(graph_ids[i]);
						}
					}

				  	for (size_t i = 0; i < ant_occurrences.size(); i++)
					{
						if (set_of_graphs && !new_measures)
						{
							ant_times.insert(pu.queryMappingSnapshotsToGraphs(ant_occurrences[i]));
						}
						else
						{
							ant_times.insert(ant_occurrences[i]);
						}
					}

					antecedent_abs_sup = (int)support(
						ant_times,
						multiple_antecedent_occurrences,
						new_measures
					);
				}
			}
			else
			{
				// we have only addition elements in the pattern
				std::cerr << "ERROR: WE SHOULDN'T HAVE ANTECEDENT WITH EMPTY ELEMENTS" << endl;
				exit(-1);

				for (int k = 0; k < max_absolute_support; k++)
				{
					ant_times.insert(k);
				}
			}

			double confidence = ((double)pattern_support_absolute) / ((double)antecedent_abs_sup);

			if (confidence >= min_confidence)
			{
			  debug_println(verbose, "SUP: ", pattern_support_absolute, ", ANT SUP: ", antecedent_abs_sup,
							", CONFIDENCE: ", confidence);
				double support_relative = (double)pattern_support_absolute / (double)max_absolute_support;
				save_pattern(pattern_edge_list, graph_ids, results, set_of_graphs, pu, pattern_support_absolute,
                             support_relative, ant_times, confidence, verbose);

				if (search_for_anomalies)
				{
					for (std::set<anomaly_pattern_with_occurrences>::iterator it = anomaly_patterns.begin(); it != anomaly_patterns.end(); ++it)
					{
						std::vector<std::array<int, PAT___SIZE>> pat_edges = (*it).anomalous_pattern_edges;
						std::set<int> occs = (*it).occurrences;

						// compute anomaly outlierness:
						std::set<int> anomaly_ant_times;
						for (std::set<int>::iterator it2 = occs.begin(); it2 != occs.end(); ++it2)
						{
							if (set_of_graphs)
							{
								anomaly_ant_times.insert(pu.queryMappingSnapshotsToGraphs(*it2));
							}
							else
							{
								anomaly_ant_times.insert(*it2);
							}
						}

						double anomaly_outlierness = 1 - ( ((double)anomaly_ant_times.size()) / ((double)antecedent_abs_sup) );

                        debug_println(verbose, "ANOMALY OUTLIERNESS: ", anomaly_outlierness);

						// save only patterns with high enough outlierness and at least one occurrence:
						if (anomaly_outlierness >=  min_anomaly_outlierness && anomaly_ant_times.size() > 0)
						{
							save_pattern_anomaly(pat_edges, occs, results_anomalies, set_of_graphs, pu,
                                                 results->saved_instances, anomaly_outlierness, verbose);
						}
					}
				}
				printResultsToFiles(results, results_anomalies, pu, output_file, set_of_graphs, compute_confidence, search_for_anomalies, true);
			}

		}
		// if no confidence (and also no anomalies)
		else
		{
			std::set<int> ant_times;
			double support_relative = (double)pattern_support_absolute / (double)max_absolute_support;
			save_pattern(pattern_edge_list, graph_ids, results, set_of_graphs, pu, pattern_support_absolute,
                         support_relative, ant_times, -1.0, verbose);
			printResultsToFiles(results, results_anomalies, pu, output_file, set_of_graphs, compute_confidence, search_for_anomalies, true);
		}

		std::set<children_candidate>::iterator it;
		for (it = children.begin(); it != children.end(); ++it)
		{
			pattern_edge_list.push_back((*it).elements);

			std::vector <int> new_graph_ids;
			std::copy((*it).occurrences.begin(), (*it).occurrences.end(), std::back_inserter(new_graph_ids));


			DGRSubgraphMining(adjacency_lists, new_graph_ids, pattern_edge_list, support_as_absolute, starting_edges, results,
                              results_anomalies, max_absolute_support, min_confidence, compute_confidence, pu,
                              ant_occurrences, set_of_graphs, search_for_anomalies, min_anomaly_outlierness,
                              output_file, verbose, new_measures);
			pattern_edge_list.pop_back();
		}
	}



	std::set<children_candidate> enumerate(std::vector<AdjacencyListCrate> &adjacency_lists, std::vector<int> &graph_ids,
                                           std::vector<std::array<int, PAT___SIZE>> &pattern_edge_list,
                                           std::vector<std::set<int>> &nodes_occupied_by_antecedent,
                                           std::vector<std::set<int>> &edges_occupied_by_antecedent, bool new_measures,
										   std::unordered_map<int, std::set<std::set<int>>> &multiple_pattern_occurrences,
										   std::unordered_map<int, std::set<std::set<int>>> &multiple_antecedent_occurrences)
	{
        // this functions is used for finding children candidates (return value)
        // adjacency_lists - adjacency list for each graph
        // graph_ids - ids of snapshots with the pattern
        // pattern_edge_list - pattern whose children are to be searched for
        // nodes_occupied_by_antecedent - (output parameter)
        // edges_occupied_by_antecedent - (output parameter)

        // here will be the result:
		std::set<children_candidate> children_candidates;

        // right-most path used for candidate generation (similarly as in gSpan)
		std::deque<int> right_most_path = find_rightmost_path(pattern_edge_list);

		// use maximum node id as a limit of number of nodes (used later)
		int number_of_nodes_in_pattern_edge_list = 0;
		for (size_t i = 0; i < pattern_edge_list.size(); i++) {
			if (number_of_nodes_in_pattern_edge_list < pattern_edge_list[i][1]) number_of_nodes_in_pattern_edge_list = pattern_edge_list[i][1];
		}
		number_of_nodes_in_pattern_edge_list++;

		// initialize nodes_occupied_by_antecedent and edges_occupied_by_antecedent vectors
		for (int i = 0; i < adjacency_lists.size(); i++) {
			nodes_occupied_by_antecedent.push_back(std::set<int>());
			edges_occupied_by_antecedent.push_back(std::set<int>());
		}

		// find occurrences in each graph
		for (int g_id : graph_ids)
		{
            // get the adjacency list (and also edge info) for the graph currently being processed
			std::vector<std::vector<int>> adj_list = adjacency_lists[g_id].adjacencyList;
			std::vector<std::vector<std::array<int, 8>>> adj_edge_info = adjacency_lists[g_id].adjacencyEdgeInfo;

			// these two vectors save src and dst vertices (their IDs) which correspond to real occurrences of the src and dst vertices of the pattern
            // i.e. it is mapping from pattern vertex ids to vertex ids in adjacency list
			std::vector<int> real_ids_src(pattern_edge_list.size(), 0);
			std::vector<int> real_ids_dst(pattern_edge_list.size(), 0);
			std::vector<int> real_ids_dst_ind(pattern_edge_list.size(), 0);
			std::vector<int> real_ids_src_ind(pattern_edge_list.size(), 0);


			int index_in_pattern_edge_list = 0;  // which element of pattern_edge_list are we currently processing

			// find occurrences in the current adj_list
            // begin by going through all edges in the adjacency list of the current graph:
			for (size_t u = 0; u < adj_list.size(); u++)
			{
				for (size_t v = 0; v < adj_list[u].size(); v++)
				{
                    // check whether the current edge in the adjacency list is equal to the current edge pattern:
					if (is_adj_info_equal_to_pattern_edge(adj_edge_info[u][v], pattern_edge_list[index_in_pattern_edge_list], true)) {
						// we found the first edge from pattern_edge_list in the adjacency list

						// which SRC and DST vertex ids (form adjacency list) were used:
						real_ids_src[0] = u;
						real_ids_dst[0] = adj_list[u][v];
                        // under which index can we find DST in the list edges going from SRC:
						real_ids_dst_ind[0] = v;
                        // and also the opposite information (under which index can we find SRC in the list of edges going from DST):
						for (size_t i = 0; i < adj_list[real_ids_dst[0]].size(); i++)
						{
							if (adj_list[real_ids_dst[0]][i] == u && is_adj_info_equal_to_pattern_edge(flipAdjacencyInfo(adj_edge_info[real_ids_dst[0]][i]), pattern_edge_list[index_in_pattern_edge_list], true))
							{
								real_ids_src_ind[0] = i;
								break;
							}
						}

						// initialize the helping variables and vectors
						// here we have the real IDs of vertices 0, 1, 2, ... from the pattern (i.e. mapping of pattern vertex ID i to real vertex ID in adjacency list can be found under index i)
						std::vector<int> sequence_of_nodes(number_of_nodes_in_pattern_edge_list, 0);
						// first edge is always forward from 0 to 1:
						sequence_of_nodes[0] = u;
						sequence_of_nodes[1] = adj_list[u][v];

                        // for each edge from pattern_edge_list, we store the index of lastly processed neighbor of the relevant node (i.e. index in adj_list[src])
						// it is used for backtracking: if we backtrack we continue from that index and not from zero.
						std::vector<int> sequence_of_edge_indices(pattern_edge_list.size(), 0);
						sequence_of_edge_indices[index_in_pattern_edge_list] = v;
						index_in_pattern_edge_list++;

						// do depth first search
						while (true) {
							// if we found the occurrence of the whole pattern:
							if (index_in_pattern_edge_list >= pattern_edge_list.size())
							{
								// save antecedent nodes and edges (i.e. those that are not addition changes) to edges_occupied_by_antecedent and edges_occupied_by_antecedent
								// (used for anomalies)
								for (int i = 0; i < pattern_edge_list.size(); i++) {
									int n_1 = real_ids_src[i];
									int n_2 = real_ids_dst_ind[i];

									// all elements of the frequent pattern should be occupied
									nodes_occupied_by_antecedent[g_id].insert(n_1);
									nodes_occupied_by_antecedent[g_id].insert(real_ids_dst[i]);
									edges_occupied_by_antecedent[g_id].insert(adj_edge_info[n_1][n_2][ADJ_INFO_ID]);
								}

							  	std::set<int> pattern_occurrence_vertexes;

							  	// save pattern occurrence and antecedent occurrence in order to properly calculate new measures
							  	if (new_measures) {
								  	std::set<int> antecedent_vertexes;
								  	for (int i = 0; i < pattern_edge_list.size(); ++i) {
									  	int n_1 = real_ids_src[i];

									  	pattern_occurrence_vertexes.insert(n_1);
									  	pattern_occurrence_vertexes.insert(real_ids_dst[i]);

									  	if (pattern_edge_list[i][PAT_SRC_CHANGETIME] != 0) {
											antecedent_vertexes.insert(n_1);
										}

									  	if (pattern_edge_list[i][PAT_DST_CHANGETIME] != 0) {
											antecedent_vertexes.insert(real_ids_dst[i]);
										}
									}

								  	multiple_pattern_occurrences[g_id].insert(pattern_occurrence_vertexes);
								  	multiple_antecedent_occurrences[g_id].insert(antecedent_vertexes);
								}

								std::vector<int> right_most_path_occurrence;
								for (size_t i = 0; i < right_most_path.size(); i++)
								{
									right_most_path_occurrence.push_back(sequence_of_nodes[right_most_path[i]]);
								}

								// generate all children (i.e. generate all edges which can be appended to the base pattern(pattern_edge_list)) and their occurrence

								// first, generate the backward edges (candidates):
								int right_most_vertex = right_most_path_occurrence.back();

								int backward_starting_ind = 0;
								if (right_most_path.back() == pattern_edge_list.back()[PAT_SRC_ID] && pattern_edge_list.back()[PAT_SRC_ID] > pattern_edge_list.back()[PAT_DST_ID])
								{
									// if the last edge in edge_list is a backward edge from the current RMP, add only edges with greater code
									for (size_t i = 0; i < right_most_path.size(); i++)
									{
										if (right_most_path[i] == pattern_edge_list.back()[PAT_DST_ID])
										{
											backward_starting_ind = i;
											break;
										}
									}
								}

								for (int i = backward_starting_ind; i < right_most_path.size() - 1; i++)
								{
									std::vector<std::array<int, 8>> edge_candidates = find_backward_edge_candidates(adj_list, adj_edge_info,
										right_most_vertex, right_most_path_occurrence[i],
										real_ids_src, real_ids_dst, real_ids_dst_ind);

									// save the edge_candidates (i,j must be appended)
									for (int j = 0; j < edge_candidates.size(); j++)
									{
										// save all children candidates
										std::array<int, 10> e_cand = { right_most_path.back(), right_most_path[i],
											edge_candidates[j][0], edge_candidates[j][1], edge_candidates[j][2], edge_candidates[j][3], edge_candidates[j][4],
											edge_candidates[j][5], edge_candidates[j][6], edge_candidates[j][7] };

										children_candidate cc;
										cc.elements = e_cand;
										// does not use edge ID when searching for the edge (we care only about the edge itself and its graph ids = occurrences)
										auto it = children_candidates.find(cc);

										if (it != std::end(children_candidates))
										{
											(it->occurrences).insert(g_id);

											if (new_measures) {
											  	it->multiple_occurrences[g_id].insert(pattern_occurrence_vertexes);
											}
										}
										else {
											cc.occurrences.insert(g_id);

											if (new_measures) {
											  	cc.multiple_occurrences[g_id].insert(pattern_occurrence_vertexes);
											}

										  	children_candidates.insert(cc);
										}
									}
								}

								// forward candidates:
								for (int i = right_most_path.size() - 1; i >= 0; i--)
								{
								  	std::vector<int> real_second_edge_ids;
								  	// realne id1 right_most_path_occurrence[i]
									std::vector<std::array<int, 8>> edge_candidates = find_forward_edge_candidates(adj_list, adj_edge_info,
										right_most_path_occurrence[i], sequence_of_nodes, real_second_edge_ids);

									for (int j = 0; j < edge_candidates.size(); j++)
									{
									  	std::set<int> child_occurrence_vertexes = pattern_occurrence_vertexes;
									  	child_occurrence_vertexes.insert(right_most_path_occurrence[i]);
									  	child_occurrence_vertexes.insert(real_second_edge_ids[j]);

										std::array<int, 10> e_cand = { right_most_path[i], right_most_path.back() + 1,
											edge_candidates[j][0], edge_candidates[j][1], edge_candidates[j][2], edge_candidates[j][3], edge_candidates[j][4],
											edge_candidates[j][5], edge_candidates[j][6], edge_candidates[j][7] };


										children_candidate cc;
										cc.elements = e_cand;
										// does not use edge ID when searching for the edge (we care only about the edge itself and its graph ids = occurrences)
										auto it = children_candidates.find(cc);


										if (it != std::end(children_candidates))
										{
											(it->occurrences).insert(g_id);

											if (new_measures) {
											  	it->multiple_occurrences[g_id].insert(child_occurrence_vertexes);
											}
										}
										else
										{
											cc.occurrences.insert(g_id);

										  	if (new_measures) {
												cc.multiple_occurrences[g_id].insert(child_occurrence_vertexes);
											}

											children_candidates.insert(cc);
										}
									}
								}

								// and backtrack
								if (index_in_pattern_edge_list < real_ids_src.size())
								{
									real_ids_src[index_in_pattern_edge_list] = 0;
									real_ids_dst[index_in_pattern_edge_list] = 0;
									real_ids_dst_ind[index_in_pattern_edge_list] = 0;
									real_ids_src_ind[index_in_pattern_edge_list] = 0;

								}
								index_in_pattern_edge_list--;

							} // if (current_index_in_edge_list >= pattern_edge_list.size()) // (if we found the occurrence of the whole pattern)

							// if we backtracked down to 1, it means that we tried all possibilities from this starting point in the given adjacency list
							if (index_in_pattern_edge_list == 0)
							{
								break;
							}

							// traverse (i.e. try to match the rest of the pattern with respect to the selected starting position in adjacency list):
							// try to find the occurrence of this pattern edge:
							int i = pattern_edge_list[index_in_pattern_edge_list][0];
							int j = pattern_edge_list[index_in_pattern_edge_list][1];

                            // check whether the edge is forward or backward
							if (i > j)
							{
								// backward edge;
								// we already have both vertices (real ids from adjacency list) in sequence_of_nodes, so get them:
								int src = sequence_of_nodes[i];
								int dst = sequence_of_nodes[j];
								// find the first index of an edge from "src" to "dst" and save the index into sequence_of_edge_indices
								bool should_backtrack = true;
								for (size_t edge_ind = sequence_of_edge_indices[index_in_pattern_edge_list]; edge_ind < adj_list[src].size(); edge_ind++)
								{
									// if we found the appropriate edge (don't use edge ID for matching):
									if (adj_list[src][edge_ind] == dst && is_adj_info_equal_to_pattern_edge(adj_edge_info[src][edge_ind], pattern_edge_list[index_in_pattern_edge_list], true))
									{
										// there can be only one edge which exactly matches
										// check if that edge is already there (occupied by traversal):
										bool is_already_there = false;
										for (size_t k = 0; k < real_ids_src.size(); k++)
										{
											if ((real_ids_src[k] == src && real_ids_dst[k] == dst && real_ids_dst_ind[k] == edge_ind)
												| (real_ids_src[k] == dst && real_ids_dst[k] == src && real_ids_src_ind[k] == edge_ind))
											{
												is_already_there = true;
												break;
											}
										}

										if (!is_already_there)
										{
											should_backtrack = false;
                                            // store the positions
											real_ids_src[index_in_pattern_edge_list] = src;
											real_ids_dst[index_in_pattern_edge_list] = dst;
											real_ids_dst_ind[index_in_pattern_edge_list] = edge_ind;
											for (size_t i = 0; i < adj_list[dst].size(); i++)
											{
												if (adj_list[dst][i] == src && is_adj_info_equal_to_pattern_edge(flipAdjacencyInfo(adj_edge_info[dst][i]), pattern_edge_list[index_in_pattern_edge_list], true))
												{
													real_ids_src_ind[index_in_pattern_edge_list] = i;
													break;
												}
											}

											sequence_of_edge_indices[index_in_pattern_edge_list] = edge_ind + 1;
											index_in_pattern_edge_list++;

											break;
										}

									}
								}

								if (should_backtrack)
								{

									sequence_of_edge_indices[index_in_pattern_edge_list] = 0;



									real_ids_src[index_in_pattern_edge_list] = 0;
									real_ids_dst[index_in_pattern_edge_list] = 0;
									real_ids_dst_ind[index_in_pattern_edge_list] = 0;
									real_ids_src_ind[index_in_pattern_edge_list] = 0;

									index_in_pattern_edge_list--;
								}
							}
							else
							{
								// forward edge
								// "src" vertex is already known in this case
								int src = sequence_of_nodes[i];
								// (but we must find the appropriate "dst" vertex)
								bool should_backtrack = true;
								for (size_t edge_ind = sequence_of_edge_indices[index_in_pattern_edge_list]; edge_ind < adj_list[src].size(); edge_ind++)
								{
									if (is_adj_info_equal_to_pattern_edge(adj_edge_info[src][edge_ind], pattern_edge_list[index_in_pattern_edge_list], true))
									{
										int dst = adj_list[src][edge_ind];
										// check if that edge is already there:
										bool is_already_there = false;
										for (size_t k = 0; k < real_ids_src.size(); k++)
										{
											if ((real_ids_src[k] == src && real_ids_dst[k] == dst && real_ids_dst_ind[k] == edge_ind)
												| (real_ids_src[k] == dst && real_ids_dst[k] == src && real_ids_src_ind[k] == edge_ind))
											{
												is_already_there = true;
												break;
											}
										}
										if (!is_already_there)
										{
											should_backtrack = false;
											real_ids_src[index_in_pattern_edge_list] = src;
											real_ids_dst[index_in_pattern_edge_list] = dst;
											real_ids_dst_ind[index_in_pattern_edge_list] = edge_ind;

											for (size_t i = 0; i < adj_list[dst].size(); i++)
											{
												if (adj_list[dst][i] == src && is_adj_info_equal_to_pattern_edge(flipAdjacencyInfo(adj_edge_info[dst][i]), pattern_edge_list[index_in_pattern_edge_list], true))
												{
													real_ids_src_ind[index_in_pattern_edge_list] = i;
													break;
												}
											}

											sequence_of_nodes[j] = dst;
											sequence_of_edge_indices[index_in_pattern_edge_list] = edge_ind + 1;
											index_in_pattern_edge_list++;

											break;
										}
									}

								}

								if (should_backtrack)
								{
									sequence_of_edge_indices[index_in_pattern_edge_list] = 0;
									real_ids_src[index_in_pattern_edge_list] = 0;
									real_ids_dst[index_in_pattern_edge_list] = 0;
									real_ids_dst_ind[index_in_pattern_edge_list] = 0;
									real_ids_src_ind[index_in_pattern_edge_list] = 0;
									index_in_pattern_edge_list--;
								}
							}

						}

					} // if (is_adj_info_equal_to_pattern_edge(adj_edge_info[u][v], pattern_edge_list[current_index_in_edge_list], true))

				} // for (size_t v = 0; v < adj_list[u].size(); v++)
			} // for (size_t u = 0; u < adj_list.size(); u++)

		} // for (int g_id : graph_ids)

		return children_candidates;
	}


	std::vector<std::array<int, 8>> find_forward_edge_candidates(std::vector<std::vector<int>> &adj_list,
		std::vector<std::vector<std::array<int, 8>>> &adj_more_info,
		int src, std::vector<int> &sequence_of_nodes, std::vector<int> &real_second_edge_ids)
	{
		std::vector<std::array<int, 8>> edge_candidates;

		for (size_t v = 0; v < adj_list[src].size(); v++)
		{
			bool is_in_sequence_of_nodes = false;
			for (size_t i = 0; i < sequence_of_nodes.size(); i++)
			{
				if (adj_list[src][v] == sequence_of_nodes[i]) {
					is_in_sequence_of_nodes = true;
					break;
				}
			}
			if (!is_in_sequence_of_nodes) {
			  	real_second_edge_ids.push_back(adj_list[src][v]);
				edge_candidates.push_back(adj_more_info[src][v]);
			}
		}
		return edge_candidates;
	}

	std::vector<std::array<int, 8>> find_backward_edge_candidates(std::vector<std::vector<int>> &adj_list,
		std::vector<std::vector<std::array<int, 8>>> &adj_more_info, int src, int dst,
		std::vector<int> &real_ids_src, std::vector<int> &real_ids_dst, std::vector<int> &real_ids_dst_ind)
	{
		std::vector<std::array<int, 8>> edge_candidates;

		std::set<int> occupied_from_the_same_direction;
		for (size_t i = 0; i < real_ids_src.size(); i++)
		{
			if (real_ids_src[i] == src && real_ids_dst[i] == dst) {
				occupied_from_the_same_direction.insert(real_ids_dst_ind[i]);
			}
		}
		std::vector<std::array<int, 8>> occupied_edges_from_other_direction;
		for (size_t i = 0; i < real_ids_src.size(); i++)
		{
			if (real_ids_src[i] == dst && real_ids_dst[i] == src) {
				occupied_edges_from_other_direction.push_back(adj_more_info[dst][real_ids_dst_ind[i]]);
			}
		}

		// do it in a reversed order, so we can find dummy first - this property is used in the .enumerate function
		for (int v = adj_list[src].size() - 1; v >= 0; v--)
		{
			//if it is the appropriate edge and it can be taken (if it is not occupied from the same direction):
			if (adj_list[src][v] == dst && occupied_from_the_same_direction.count(v) == 0)
			{
				bool can_be_used = true;
				for (size_t i = 0; i < occupied_edges_from_other_direction.size(); i++)
				{
					if (is_adj_info_equal_to(adj_more_info[src][v], flipAdjacencyInfo(occupied_edges_from_other_direction[i]), false))
					{
						can_be_used = false;
						break;
					}
				}
				if (can_be_used)
				{
					edge_candidates.push_back(adj_more_info[src][v]);
				}
			}
		}
		return edge_candidates;
	}


	void save_pattern(std::vector<std::array<int, 10>> &edge_list, std::vector<int> &graph_ids, results_crate * results,
                      bool set_of_graphs, PartialUnion pu, int pattern_support_absolute, double support_relative,
                      std::set<int> &antecedent_occurrences, double confidence = -1.0, bool verbose = false)
	{
		results->saved_instances++;

		std::set<int> already_saved_node_ids;
        debug_println(verbose, "FREQUENT PATTERN ", results->saved_instances, ", SUP: ", support_relative, " (",
                      pattern_support_absolute, " INSTANCES), CONF: ", confidence);

		results->support_absolute.push_back(pattern_support_absolute);
		results->support.push_back(support_relative);
		if (confidence >= 0)
		{
			results->confidence.push_back(confidence);
		}


		for (size_t i = 0; i < edge_list.size(); i++)
		{
			if (already_saved_node_ids.count(edge_list[i][0]) == 0)
			{
				already_saved_node_ids.insert(edge_list[i][0]);
				std::array<int, 4> result_node = { results->saved_instances, edge_list[i][0], edge_list[i][2], edge_list[i][3] };
				results->result_nodes.push_back(result_node);
			}
			if (already_saved_node_ids.count(edge_list[i][1]) == 0)
			{
				already_saved_node_ids.insert(edge_list[i][1]);
				std::array<int, 4> result_node = { results->saved_instances, edge_list[i][1], edge_list[i][7], edge_list[i][8] };
				results->result_nodes.push_back(result_node);
			}

			std::array<int, 6> result_edge = { results->saved_instances, edge_list[i][0], edge_list[i][1], edge_list[i][5], edge_list[i][4], edge_list[i][6] };
			results->result_edges.push_back(result_edge);
		}

		if (set_of_graphs)
		{
			std::set<int> occurrences_set;
			for (int j = 0; j < graph_ids.size(); j++)
			{
				occurrences_set.insert(pu.queryMappingSnapshotsToGraphs(graph_ids[j]));
			}
			std::vector<int> occurrences_vector(occurrences_set.begin(), occurrences_set.end());
			results->occurrences.push_back(occurrences_vector);
		}
		else
		{

			std::vector<int> new_(graph_ids);
			results->occurrences.push_back(new_);
		}
	}


	void save_pattern_anomaly(std::vector<std::array<int, PAT___SIZE>> &edge_list, std::set<int> &graph_ids,
                              results_crate_anomalies * results_anomaly, bool set_of_graphs, PartialUnion pu,
                              int anomaly_id_of_explanation_pattern, double anomaly_outlierness,
                              bool verbose = false)
	{
		results_anomaly->anomaly_saved_instances++;

        debug_println(verbose, "ANOMALY PATTERN ", results_anomaly->anomaly_saved_instances, ", OUTLIERNESS: ",
                      anomaly_outlierness);

		//anomaly_result_edges, anomaly_result_nodes
		std::set<int> already_saved_node_ids;
		for (size_t i = 0; i < edge_list.size(); i++)
		{
			if (already_saved_node_ids.count(edge_list[i][0]) == 0)
			{
				already_saved_node_ids.insert(edge_list[i][0]);
				std::array<int, 4> result_node = { results_anomaly->anomaly_saved_instances, edge_list[i][0], edge_list[i][2], edge_list[i][3] };
				results_anomaly->anomaly_result_nodes.push_back(result_node);
			}
			if (already_saved_node_ids.count(edge_list[i][1]) == 0)
			{
				already_saved_node_ids.insert(edge_list[i][1]);
				std::array<int, 4> result_node = { results_anomaly->anomaly_saved_instances, edge_list[i][1], edge_list[i][7], edge_list[i][8] };
				results_anomaly->anomaly_result_nodes.push_back(result_node);
			}

			std::array<int, 6> result_edge = { results_anomaly->anomaly_saved_instances, edge_list[i][0], edge_list[i][1], edge_list[i][5], edge_list[i][4], edge_list[i][6] };
			results_anomaly->anomaly_result_edges.push_back(result_edge);
		}


		//anomaly_id_of_anomalous_pattern
		results_anomaly->anomaly_id_of_anomalous_pattern.push_back(results_anomaly->anomaly_saved_instances);

		//anomaly_id_of_explanation_pattern
		results_anomaly->anomaly_id_of_explanation_pattern.push_back(anomaly_id_of_explanation_pattern);

		//anomaly_outlierness
		results_anomaly->anomaly_outlierness.push_back(anomaly_outlierness);

		//anomaly_occurrences
		if (set_of_graphs)
		{
			std::set<int> occurrences_set;
			for (std::set<int>::iterator it = graph_ids.begin(); it != graph_ids.end(); ++it)
				//for (int j = 0; j < graph_ids.size(); j++)
			{
				occurrences_set.insert(pu.queryMappingSnapshotsToGraphs(*it));
			}
			std::vector<int> occurrences_vector(occurrences_set.begin(), occurrences_set.end());
			results_anomaly->anomaly_occurrences.push_back(occurrences_vector);
		}
		else
		{
			std::vector<int> new_;
			std::copy(graph_ids.begin(), graph_ids.end(), std::back_inserter(new_));
			results_anomaly->anomaly_occurrences.push_back(new_);
		}

	}


	void compute_antecedent_pattern_from_pattern(std::vector<std::array<int, PAT___SIZE>> &edge_list,
												 std::vector<std::array<int, PAT___SIZE>> &antecedent_pattern_edges,
												 std::vector<int> &isolated_nodes_label,
												 std::vector<int> &isolated_nodes_changetime,
												 std::vector<int> &isolated_nodes_id,
												 std::vector<std::array<int, PAT___SIZE>> &antecedent_pattern_edges_ADDED,
												 std::vector<int> &antecedent_pattern_node_id_ADDED,
												 std::vector<int> &antecedent_pattern_node_labels_ADDED)
	{
		std::vector<int> isolated_nodes_label_local;
		std::vector<int> isolated_nodes_changetime_local;
		std::vector<int> isolated_nodes_id_local;


		// first, save non-addition nodes and edges, and addition edges
		for (size_t i = 0; i < edge_list.size(); i++)
		{
			// if the first node was added
			if (edge_list[i][PAT_SRC_CHANGETIME] == 0)
			{
				// if just the first node was added
				if (edge_list[i][PAT_DST_CHANGETIME] != 0)
				{
					// add non-addition node as isolated
					isolated_nodes_id_local.push_back(edge_list[i][PAT_DST_ID]);
					isolated_nodes_label_local.push_back(edge_list[i][PAT_DST_LABEL]);
					isolated_nodes_changetime_local.push_back(edge_list[i][PAT_DST_CHANGETIME]);
				}

				// and also add the addition edge (it must be with addition node) to antecedent_pattern_edges_ADDED
				std::array<int, PAT___SIZE> newedge(edge_list[i]);
				antecedent_pattern_edges_ADDED.push_back(newedge);

			}
			// else if first wasn't added
			else
			{
				// and only the second node was added:
				if (edge_list[i][PAT_DST_CHANGETIME] == 0)
				{
					// put the first node as isolated
					isolated_nodes_id_local.push_back(edge_list[i][PAT_SRC_ID]);
					isolated_nodes_label_local.push_back(edge_list[i][PAT_SRC_LABEL]);
					isolated_nodes_changetime_local.push_back(edge_list[i][PAT_SRC_CHANGETIME]);

					// and also add the addition edge
					std::array<int, PAT___SIZE> newedge(edge_list[i]);
					antecedent_pattern_edges_ADDED.push_back(newedge);
				}
				// none of the nodes was added
				else
				{
					// if just the edge was added, 
					if (edge_list[i][PAT_CHANGETIME] == 0)
					{
						// add both nodes as isolated (they are definitely not dummy):
						isolated_nodes_id_local.push_back(edge_list[i][PAT_SRC_ID]);
						isolated_nodes_label_local.push_back(edge_list[i][PAT_SRC_LABEL]);
						isolated_nodes_changetime_local.push_back(edge_list[i][PAT_SRC_CHANGETIME]);
						isolated_nodes_id_local.push_back(edge_list[i][PAT_DST_ID]);
						isolated_nodes_label_local.push_back(edge_list[i][PAT_DST_LABEL]);
						isolated_nodes_changetime_local.push_back(edge_list[i][PAT_DST_CHANGETIME]);

						// and the edge as added:
						std::array<int, PAT___SIZE> newedge(edge_list[i]);
						antecedent_pattern_edges_ADDED.push_back(newedge);
					}
					// nothing was added
					else
					{
						std::array<int, PAT___SIZE> newedge(edge_list[i]);
						antecedent_pattern_edges.push_back(newedge);

					}
				}
			}
		}

		// get node_ids which are part of the antecedent_pattern_edges (those which are part of antecedent_pattern_edges_ADDED are not removed!)
		std::set<int> node_ids_in_new_edge_list;
		for (size_t i = 0; i < antecedent_pattern_edges.size(); i++)
		{
			node_ids_in_new_edge_list.insert(antecedent_pattern_edges[i][0]);
			node_ids_in_new_edge_list.insert(antecedent_pattern_edges[i][1]);
		}


		// now take only those isolated nodes, which are not part of any edges and also drop duplicates
		std::set<int> node_ids_already_taken;

		for (size_t i = 0; i < isolated_nodes_id_local.size(); i++)
		{
			bool is_already_taken = node_ids_already_taken.find(isolated_nodes_id_local[i]) != node_ids_already_taken.end();
			bool is_in_edges = node_ids_in_new_edge_list.find(isolated_nodes_id_local[i]) != node_ids_in_new_edge_list.end();


			if (!is_already_taken && !is_in_edges)
			{
				node_ids_already_taken.insert(isolated_nodes_id_local[i]);

				isolated_nodes_id.push_back(isolated_nodes_id_local[i]);
				isolated_nodes_label.push_back(isolated_nodes_label_local[i]);
				isolated_nodes_changetime.push_back(isolated_nodes_changetime_local[i]);
			}

		}


		// save all addition nodes separately
		std::vector<int> antecedent_pattern_isolated_node_id_ADDED;
		std::vector<int> antecedent_pattern_isolated_node_labels_ADDED;
		std::set<int> ids_of_processed_nodes_ADDED;

		bool is_already_taken;
		for (size_t i = 0; i < antecedent_pattern_edges_ADDED.size(); i++)
		{
			// take the SRC node only if it is not taken yet and it is an addtion
			is_already_taken = ids_of_processed_nodes_ADDED.find(antecedent_pattern_edges_ADDED[i][PAT_SRC_ID]) != ids_of_processed_nodes_ADDED.end();
			if (!is_already_taken && antecedent_pattern_edges_ADDED[i][PAT_SRC_CHANGETIME] == 0)
			{
				antecedent_pattern_node_id_ADDED.push_back(antecedent_pattern_edges_ADDED[i][PAT_SRC_ID]);
				antecedent_pattern_node_labels_ADDED.push_back(antecedent_pattern_edges_ADDED[i][PAT_SRC_LABEL]);
				ids_of_processed_nodes_ADDED.insert(antecedent_pattern_edges_ADDED[i][PAT_SRC_ID]);
			}

			// take the DST node only if it is not taken yet and it is an addtion
			is_already_taken = ids_of_processed_nodes_ADDED.find(antecedent_pattern_edges_ADDED[i][PAT_DST_ID]) != ids_of_processed_nodes_ADDED.end();
			if (!is_already_taken && antecedent_pattern_edges_ADDED[i][PAT_DST_CHANGETIME] == 0)
			{
				antecedent_pattern_node_id_ADDED.push_back(antecedent_pattern_edges_ADDED[i][PAT_DST_ID]);
				antecedent_pattern_node_labels_ADDED.push_back(antecedent_pattern_edges_ADDED[i][PAT_DST_LABEL]);
				ids_of_processed_nodes_ADDED.insert(antecedent_pattern_edges_ADDED[i][PAT_DST_ID]);
			}
		}


	}


	inline bool operator<(const node_label_changetime& lhs, const node_label_changetime& rhs)
	{
		return (lhs.label < rhs.label || (lhs.label == rhs.label && lhs.changetime < rhs.changetime));
	}
	inline bool operator==(const node_label_changetime& lhs, const node_label_changetime& rhs)
	{
		return (lhs.label == rhs.label &&  lhs.changetime == rhs.changetime);
	}


	// for a given mapping of nodes (both addition and non-addition - where non-addition can be dummy),
	// it finds mapping of addition edges.
	// the result is in edge_mapping_sets, where we save mappings of only non-dummy addition edges
	void find_addition_edges(AdjacencyListCrate & adjacency_list, std::vector<std::array<int, PAT___SIZE>> & antecedent_pattern_edges_ADDED,
		std::map<char, int> & from_pattern_id_to_adj_id_map, int num_of_nondummy_nodes, std::vector<std::set<int_pair>> & edge_mapping_sets,
		std::set<int> & edges_occupied_by_antecedent, bool debug)
	{
		std::vector<std::vector<int>> adj_list = adjacency_list.adjacencyList;
		std::vector<std::vector<std::array<int, ADJ_INFO___SIZE>>> adj_more_info = adjacency_list.adjacencyEdgeInfo;

		int special_symbol_for_max_index = std::numeric_limits<int>::max();

		int number_of_created_dummy_edges = 0;

		int current_index = 0; // which of the antecedent_pattern_edges_ADDED are we currently processing

		std::vector<int> selected_ids_from_adj; // which IDS of adj nodes correspond to those in antecedent_pattern_isolated_node_id
		std::vector<bool> is_the_used_edge_dummy;
		std::vector<int> which_indices_should_be_used_next;

		std::set<int> which_edge_ids_are_used;

		which_indices_should_be_used_next.push_back(0); // for the first addition edge, we want to start from the first neighbor in the adjacency list

		// DO DFS:
		while (true)
		{

			if (debug) cout << "CURRENT INDEX: " << current_index << endl;
			// if all ADDITION edges were mapped:
			if (antecedent_pattern_edges_ADDED.size() <= current_index)
			{
                // WE FOUND ALL EDGES (pattern --> adjacency) antecedent_pattern_edges_ADDED[i][PAT_ID] --> selected_ids_from_adj[i]

				// save all pairs PATTERN_EDGE_ID->ADJ_EDGE_ID for non-dummy edges
				std::set<int_pair> edge_mapping_set;
				for (size_t i = 0; i < selected_ids_from_adj.size(); i++)
				{
					if (!is_the_used_edge_dummy[i])
					{
						int_pair edge_mapping_pair;
						edge_mapping_pair.first = antecedent_pattern_edges_ADDED[i][PAT_ID];
						edge_mapping_pair.second = selected_ids_from_adj[i];
						edge_mapping_set.insert(edge_mapping_pair);
					}
				}
				// now save this edge_mapping_set (then we check if it is not a subset of another mapping.. and remove all subsets of this mapping)
				edge_mapping_sets.push_back(edge_mapping_set);

				// AND BACKTRACK:

				bool was_the_edge_dummy = is_the_used_edge_dummy.back();

				which_indices_should_be_used_next.pop_back();
				if (was_the_edge_dummy)
				{
					number_of_created_dummy_edges--;
				}
				else
				{
					which_edge_ids_are_used.erase(selected_ids_from_adj.back());
				}

				is_the_used_edge_dummy.pop_back();
				selected_ids_from_adj.pop_back();
				current_index--;
			}

			int src_id = from_pattern_id_to_adj_id_map[antecedent_pattern_edges_ADDED[current_index][PAT_SRC_ID]];
			int dst_id = from_pattern_id_to_adj_id_map[antecedent_pattern_edges_ADDED[current_index][PAT_DST_ID]];
			// all non_dummy nodes have ID < num_of_nondummy_nodes, so we can check which nodes are dummy by:
			bool src_is_dummy = src_id >= num_of_nondummy_nodes;
			bool dst_is_dummy = dst_id >= num_of_nondummy_nodes;

			// if at least one node of the edge is dummy, then the edge itself is dummy
			if (src_is_dummy || dst_is_dummy)
			{
				// and if we didn't just try it as dummy:
				if (which_indices_should_be_used_next[current_index] != special_symbol_for_max_index)
				{
					// map it as dummy:
					which_indices_should_be_used_next[current_index] = special_symbol_for_max_index;
					which_indices_should_be_used_next.push_back(0);

					is_the_used_edge_dummy.push_back(true);

					selected_ids_from_adj.push_back(-1); // push there arbitrary number (the number is not used)

					number_of_created_dummy_edges++;

					current_index++;
				}
			}
			// if no node of the edge is dummy, then the edge itself can be dummy but doesn't have to be
			else
			{
				bool should_try_dummy = true;

				int src_index = which_indices_should_be_used_next[current_index];

				for (; src_index < adj_list[src_id].size(); src_index++)
				{
					if (adj_list[src_id][src_index] == dst_id)
					{
						if (debug) cout << "FOUND POSSIBLE EDGE " << src_id << ", " << dst_id << endl;
						bool matches = is_adj_info_equal_to_pattern_edge(adj_more_info[src_id][src_index], antecedent_pattern_edges_ADDED[current_index], true);
						bool was_already_used = which_edge_ids_are_used.find(adj_more_info[src_id][src_index][ADJ_INFO_ID]) != which_edge_ids_are_used.end();

						bool is_edge_occcupied = edges_occupied_by_antecedent.find(adj_more_info[src_id][src_index][ADJ_INFO_ID]) != edges_occupied_by_antecedent.end();

						if (matches && !was_already_used && !is_edge_occcupied)
						{
							if (debug) cout << "IT MATCHES" << endl;

							should_try_dummy = false;


							which_indices_should_be_used_next[current_index] = src_index + 1;
							which_indices_should_be_used_next.push_back(0);

							is_the_used_edge_dummy.push_back(false);
							selected_ids_from_adj.push_back(adj_more_info[src_id][src_index][ADJ_INFO_ID]);

							which_edge_ids_are_used.insert(adj_more_info[src_id][src_index][ADJ_INFO_ID]);
							current_index++;
							break;
						}
					}
				}


				// if we tried all regular edges and should backtrack, try to map to a dummy edge first:
				if (should_try_dummy && which_indices_should_be_used_next[current_index] != special_symbol_for_max_index)
				{
					//which_indices_should_be_used_next[current_index] = adj_list[from_id].size() + 1;
					which_indices_should_be_used_next[current_index] = special_symbol_for_max_index;
					which_indices_should_be_used_next.push_back(0);

					is_the_used_edge_dummy.push_back(true);

					selected_ids_from_adj.push_back(-1); // push there arbitrary number (the number is not used)

					number_of_created_dummy_edges++;

					current_index++;
				}
			}

			if (current_index != 0 && which_indices_should_be_used_next[current_index] == special_symbol_for_max_index)
			{
				// now we also tried the dummy one, so really backtrack
				bool was_the_edge_dummy = is_the_used_edge_dummy.back();

				which_indices_should_be_used_next.pop_back();
				if (was_the_edge_dummy)
				{
					number_of_created_dummy_edges--;
				}
				else
				{
					which_edge_ids_are_used.erase(selected_ids_from_adj.back());
				}

				is_the_used_edge_dummy.pop_back();
				selected_ids_from_adj.pop_back();
				current_index--;
			}

			if (current_index == 0 && which_indices_should_be_used_next[current_index] == special_symbol_for_max_index)
			{
				// we tried all combinations, just end:
				break;
			}

		}

	}


	void find_addition_nodes_and_edges(AdjacencyListCrate & adjacency_list, std::vector<std::array<int, PAT___SIZE>> & antecedent_pattern_edges_ADDED,
		std::vector<int> & antecedent_pattern_node_id_ADDED,
		std::vector<int> & antecedent_pattern_node_labels_ADDED,
		std::map<char, int> & from_pattern_id_to_adj_id_map,
		std::vector<std::array<int, ADJ_NODES___SIZE>> & adj_list_nodes,
		std::set<non_dummy_vertices_and_edges> & sets_of_used_edges,
		std::set<int> & nodes_occupied_by_antecedent, std::set<int> & edges_occupied_by_antecedent,
		bool debug)
	{
		// first, try to find the addition nodes:

		std::vector<int> selected_ids_from_adj; // which IDS of adj nodes correspond to those in antecedent_pattern_isolated_node_id
		std::vector<bool> is_the_used_node_dummy;
		std::vector<int> which_indices_should_be_used_next; // here we save the currently used indices for backtracking
		std::vector<bool> adjacency_nodes_are_used(adj_list_nodes.size(), false); // here we save which nodes from the adjacency list are already used

		int number_of_created_dummy_vertices = 0;

		int isolated_vertex_current_index_ADDED = 0; // we want to process the the first isolated vertex
		which_indices_should_be_used_next.push_back(0); // for the first isolated node, we want to start from the first node of adjacency list

		// DO DFS:
		while (true)
		{

			// if all isolated ADDITION nodes were mapped:
			if (antecedent_pattern_node_labels_ADDED.size() <= isolated_vertex_current_index_ADDED)
			{
				// we don't need to check whether there are no addition edges - we wouldn't get here from the outer function
				// process the addition edges:

				// CREATE MAPPING: PATTERN ANTECEDENT IDS -> selected_ids_from_adj
				for (size_t i = 0; i < antecedent_pattern_node_id_ADDED.size(); i++)
				{
					from_pattern_id_to_adj_id_map[antecedent_pattern_node_id_ADDED[i]] = selected_ids_from_adj[i];
				}
				// FINALLY, FIND MAPPING OF ADDITION EDGES:
				std::vector<std::set<int_pair>> edge_mapping_sets;
				find_addition_edges(adjacency_list, antecedent_pattern_edges_ADDED, from_pattern_id_to_adj_id_map,
					adj_list_nodes.size(), edge_mapping_sets, edges_occupied_by_antecedent, debug);

				std::vector<std::vector<int_pair>> maximal_set_of_edges = find_maximal_subsets_of_pairs(edge_mapping_sets);

				// get the set of non_dummy_vertices:
				std::set<int> set_of_non_dummy_vertices;
				for (size_t i = 0; i < antecedent_pattern_node_id_ADDED.size(); i++)
				{
					if (!is_the_used_node_dummy[i])
					{
						set_of_non_dummy_vertices.insert(antecedent_pattern_node_id_ADDED[i]);
					}
				}

				// SAVE the used pattern edges to a set
				for (size_t i = 0; i < maximal_set_of_edges.size(); i++)
				{
					std::set<int> set_of_used_edges;
					for (size_t j = 0; j < maximal_set_of_edges[i].size(); j++)
					{
						set_of_used_edges.insert(maximal_set_of_edges[i][j].first);
					}
					non_dummy_vertices_and_edges ndve;
					ndve.non_dummy_vertices = set_of_non_dummy_vertices;
					ndve.non_dummy_edges = set_of_used_edges;

					sets_of_used_edges.insert(ndve);
				}

				// if there were no ADDITION ISOLATED NODES, just finish the DFS:
				if (antecedent_pattern_node_id_ADDED.size() == 0) {
					break;
				}

				// else BACKTRACK:

				bool was_the_node_dummy = is_the_used_node_dummy.back();
				which_indices_should_be_used_next.pop_back();

				if (was_the_node_dummy)
				{
					number_of_created_dummy_vertices--;
				}
				else
				{
					adjacency_nodes_are_used[which_indices_should_be_used_next.back() - 1] = false;
				}
				is_the_used_node_dummy.pop_back();
				selected_ids_from_adj.pop_back();
				isolated_vertex_current_index_ADDED--;

			}
            // Nodes were not matched yet

			bool should_try_dummy = true;
			// try to find matching addition node:
			int node_candidate_index = which_indices_should_be_used_next[isolated_vertex_current_index_ADDED];
			for (; node_candidate_index < adj_list_nodes.size(); node_candidate_index++)
			{
				bool is_occupied_by_frequent_pattern = nodes_occupied_by_antecedent.find(adj_list_nodes[node_candidate_index][ADJ_NODES_ID]) != nodes_occupied_by_antecedent.end();
				// if the current adjacency node wasn't used yet:
				if (!adjacency_nodes_are_used[node_candidate_index] && !is_occupied_by_frequent_pattern)
				{
					// if it is same as that isolated vertex:
					// (we must take the antecedent info of both nodes)
					bool are_same = adj_list_nodes[node_candidate_index][ADJ_NODES_LABEL] == antecedent_pattern_node_labels_ADDED[isolated_vertex_current_index_ADDED];

					if (are_same)
					{
						//if (debug) cout << "Found node match: " << isolated_vertex_current_index_ADDED <<
						//	"-th addition vertex to adj. vertex with ID: " << adj_list_nodes[node_candidate_index][ADJ_NODES_ID] << endl;
						//cout << "DOING FINDING MATCHING: " << endl;

						should_try_dummy = false;
						which_indices_should_be_used_next[isolated_vertex_current_index_ADDED] = node_candidate_index + 1;
						which_indices_should_be_used_next.push_back(0);
						is_the_used_node_dummy.push_back(false);
						selected_ids_from_adj.push_back(adj_list_nodes[node_candidate_index][ADJ_NODES_ID]);

						adjacency_nodes_are_used[node_candidate_index] = true;
						isolated_vertex_current_index_ADDED++;
						break;
					}
				}
			}
			// if we tried all regular nodes and should backtrack, try to map to a dummy vertex first:
			if (should_try_dummy && which_indices_should_be_used_next[isolated_vertex_current_index_ADDED] <= adj_list_nodes.size())
			{
				//if (debug) cout << "Matching " << isolated_vertex_current_index_ADDED <<
				//	"-th addition vertex to a new dummy vertex with ID: " << (adj_list_nodes.size() + number_of_created_dummy_vertices) << endl;

				which_indices_should_be_used_next[isolated_vertex_current_index_ADDED] = adj_list_nodes.size() + 1; // just mark there the size + 1
				which_indices_should_be_used_next.push_back(0);
				is_the_used_node_dummy.push_back(true);
				// selected_ids_from_adj.push_back(adj_list_nodes[node_candidate_index][ADJ_NODES_ID]);
				//mark down the new ID of the dummy:
				//cout << "USING adj_list_nodes.size() = " << adj_list_nodes.size() << ", number_of_created_dummy_vertices = " << number_of_created_dummy_vertices << endl;

				selected_ids_from_adj.push_back(adj_list_nodes.size() + number_of_created_dummy_vertices);
				number_of_created_dummy_vertices++;
				isolated_vertex_current_index_ADDED++;
			}


			if (isolated_vertex_current_index_ADDED > 0 && which_indices_should_be_used_next[isolated_vertex_current_index_ADDED] > adj_list_nodes.size())
			{
				// BACKTRACK:
				// now we also tried the dummy one, so really backtrack

				bool was_the_node_dummy = is_the_used_node_dummy.back();

				which_indices_should_be_used_next.pop_back();
				if (was_the_node_dummy)
				{
					number_of_created_dummy_vertices--;
				}
				else
				{
					adjacency_nodes_are_used[which_indices_should_be_used_next.back() - 1] = false;
				}

				//cout << "DOING FINAL BACKTRACK: " << endl;

				is_the_used_node_dummy.pop_back();
				selected_ids_from_adj.pop_back();
				isolated_vertex_current_index_ADDED--;
			}

			if (isolated_vertex_current_index_ADDED == 0 && which_indices_should_be_used_next[isolated_vertex_current_index_ADDED] > adj_list_nodes.size())
			{
				// we tried all combinations, just end:
				break;
			}
		}
	}


	// this function tries to map all isolated vertices to some vertices in adjacency list
	// it it succeeds, it will try to map also the "addition" edges
	void find_isolated_nodes_and_addition_edges(AdjacencyListCrate & adjacency_list, std::vector<std::array<int, PAT___SIZE>> & antecedent_pattern_edges_ADDED,
		std::vector<int> & antecedent_pattern_node_id_ADDED,
		std::vector<int> & antecedent_pattern_node_labels_ADDED,
		std::vector<int> & antecedent_pattern_isolated_node_id,
		std::vector<int> & antecedent_pattern_isolated_node_labels,
		std::vector<int> & antecedent_pattern_isolated_node_changetime, PartialUnion pu, bool search_for_anomalies,
		std::set<int> & nodes_occupied_by_antecedent, std::set<int> & edges_occupied_by_antecedent,
		std::set<int> & occupied_node_ids_in_adj_list,
		int g_id,
		std::vector<std::array<int, PAT___SIZE>> & anomaly_pattern_edges,
		std::set<anomaly_pattern_with_occurrences> & anomaly_patterns,
		bool debug)
	{
		std::vector<std::array<int, ADJ_NODES___SIZE>> adj_list_nodes = adjacency_list.nodes;
		std::vector<int> selected_ids_from_adj; // which IDS of adj nodes correspond to those in antecedent_pattern_isolated_node_id
		std::vector<int> which_indices_were_used; // here we save the currently used indices for backtracking
		std::vector<bool> adjacency_nodes_are_used(adj_list_nodes.size(), false); // here we save which nodes from the adjacency list are already used

		// DO DFS:

		int isolated_vertex_current_index = 0; // we want to process the the first isolated vertex
		which_indices_were_used.push_back(0); // for the first isolated node, we want to start from the first node of adjacency list

		while (true)
		{
			// if all isolated nodes were mapped (found):
			if (antecedent_pattern_isolated_node_id.size() <= isolated_vertex_current_index)
			{
				// and there are no addition edges (and thus also no addition nodes because of the connectedness of the pattern), just save the pattern:
				if (antecedent_pattern_edges_ADDED.size() == 0)
				{
					std::vector<std::array<int, PAT___SIZE>> anomaly_pattern_edges_all;
					for (size_t i = 0; i < anomaly_pattern_edges.size(); i++)
					{
						anomaly_pattern_edges_all.push_back(anomaly_pattern_edges[i]);
					}
					// save the anomalous pattern with its occurrences - only regular edges
					anomaly_pattern_with_occurrences ap;
					ap.anomalous_pattern_edges = anomaly_pattern_edges_all;
					ap.minimum_code = compute_min_code(anomaly_pattern_edges_all);
					auto it = anomaly_patterns.find(ap);
					if (it != std::end(anomaly_patterns))
					{

						(it->occurrences).insert(g_id);
					}
					else
					{
						ap.occurrences.insert(g_id);
						anomaly_patterns.insert(ap);
					}
				}
				// but if there are also some addition edges (possibly with addition nodes), try to find them:
				else
				{
					std::map<char, int> from_pattern_id_to_adj_id_map;
					for (size_t i = 0; i < antecedent_pattern_isolated_node_id.size(); i++)
					{
						from_pattern_id_to_adj_id_map[antecedent_pattern_isolated_node_id[i]] = selected_ids_from_adj[i];
					}

					// OUTPUT of find_addition_nodes_and_edges will be here:
					std::set<non_dummy_vertices_and_edges> sets_of_used_edges;
					find_addition_nodes_and_edges(adjacency_list, antecedent_pattern_edges_ADDED, antecedent_pattern_node_id_ADDED, antecedent_pattern_node_labels_ADDED, from_pattern_id_to_adj_id_map,
						adj_list_nodes, sets_of_used_edges, nodes_occupied_by_antecedent, edges_occupied_by_antecedent, debug);
					// process sets_of_used_edges (keep only maximal subsets)
					std::vector<non_dummy_vertices_and_edges> vector_of_edge_sets;
					for (set< non_dummy_vertices_and_edges >::iterator new_it = sets_of_used_edges.begin(); new_it != sets_of_used_edges.end(); new_it++)
					{
						vector_of_edge_sets.push_back(*new_it);
					}
					// here we get the IDs of non-dummy pattern edges and nodes; only maximal subsets (all other IDs are dummy)

					std::vector<non_dummy_vertices_and_edges> maximal_patterns_of_addition_edges = find_maximal_subsets_of_ndve(vector_of_edge_sets);

					// now for each such maximal set of addition patterns, create one anomaly pattern:
					for (size_t i = 0; i < maximal_patterns_of_addition_edges.size(); i++)
					{
						std::vector<std::array<int, PAT___SIZE>> anomaly_pattern_edges_all;
						for (size_t i = 0; i < anomaly_pattern_edges.size(); i++)
						{
							anomaly_pattern_edges_all.push_back(anomaly_pattern_edges[i]);
						}

						// add addition edges to anomaly_pattern_edges_all:
						// we need to mark dummy vertices and edges:

						int num_of_dummy_elements = 0;

                        // checking these addition edges:
						for (size_t j = 0; j < antecedent_pattern_edges_ADDED.size(); j++)
						{
							std::array<int, PAT___SIZE> anomaly_edge;
							// first just copy the edge as it is:
							for (size_t k = 0; k < PAT___SIZE; k++)
							{
								anomaly_edge[k] = antecedent_pattern_edges_ADDED[j][k];
							}
							
							// then, if there are some dummy elements, modify them:
							// if they were "addition nodes" then we must check it by using the results from find_addition_nodes_and_edges()
							bool is_src_vertex_nondummy = maximal_patterns_of_addition_edges[i].non_dummy_vertices.find(antecedent_pattern_edges_ADDED[j][PAT_SRC_ID]) != maximal_patterns_of_addition_edges[i].non_dummy_vertices.end();
							bool is_dst_vertex_nondummy = maximal_patterns_of_addition_edges[i].non_dummy_vertices.find(antecedent_pattern_edges_ADDED[j][PAT_DST_ID]) != maximal_patterns_of_addition_edges[i].non_dummy_vertices.end();
							bool is_edge_nondummy = maximal_patterns_of_addition_edges[i].non_dummy_edges.find(antecedent_pattern_edges_ADDED[j][PAT_ID]) != maximal_patterns_of_addition_edges[i].non_dummy_edges.end();

							// if they are not "addition" nodes, then it is clear that these nodes are not dummy:
							is_src_vertex_nondummy = is_src_vertex_nondummy || (antecedent_pattern_edges_ADDED[j][PAT_SRC_CHANGETIME] != 0);
							is_dst_vertex_nondummy = is_dst_vertex_nondummy || (antecedent_pattern_edges_ADDED[j][PAT_DST_CHANGETIME] != 0);

							if (!is_src_vertex_nondummy) anomaly_edge[PAT_SRC_LABEL] = pu.getDummyLabel(anomaly_edge[PAT_SRC_LABEL]);
							if (!is_dst_vertex_nondummy) anomaly_edge[PAT_DST_LABEL] = pu.getDummyLabel(anomaly_edge[PAT_DST_LABEL]);
							if (!is_edge_nondummy) anomaly_edge[PAT_LABEL] = pu.getDummyLabel(anomaly_edge[PAT_LABEL]);

							num_of_dummy_elements += (!is_src_vertex_nondummy + !is_dst_vertex_nondummy + !is_edge_nondummy);

							anomaly_pattern_edges_all.push_back(anomaly_edge);
						}

						// if there are no regular edges from the antecedent and all "ADDITION" edges contain only DUMMY elements, then the pattern is pure "ADDITION-DUMMY"
						bool is_only_addition_pattern = (anomaly_pattern_edges.size() == 0) && (antecedent_pattern_edges_ADDED.size() * 3 == num_of_dummy_elements);

						// we cannot save patterns which are pure "ADDITION-DUMMY" if the current snapshot contain the frequent pattern
						// (more precisely, we don't save the occurrence)
						if (!is_only_addition_pattern || nodes_occupied_by_antecedent.size() == 0)
						{
							// and now save the anomalous pattern with its occurrences 
							anomaly_pattern_with_occurrences ap;
							ap.anomalous_pattern_edges = anomaly_pattern_edges_all;
							ap.minimum_code = compute_min_code(anomaly_pattern_edges_all);
							auto it = anomaly_patterns.find(ap);
							if (it != std::end(anomaly_patterns))
							{

								(it->occurrences).insert(g_id);
							}
							else
							{
								ap.occurrences.insert(g_id);
								anomaly_patterns.insert(ap);
							}
						}


					}
				}
                // ALL FOUND ISOLATED VERTICES PROCESSED

				// if there were no isolated vertices, just end after all anomaly patterns were saved
				if (antecedent_pattern_isolated_node_id.size() == 0)
				{
					return;
				}

				// NOW WE may try different solution
			}

			// try to map next isolated node:
			bool should_backtrack = true;
			for (int node_candidate_index = which_indices_were_used[isolated_vertex_current_index]; node_candidate_index < adj_list_nodes.size(); node_candidate_index++)
			{
				// if the current adjacency node wasn't used yet, AND it ins't occupied by the mapped antecedent edges, AND it is not occupied by the FREQUENT PATTERN antecedent:
				bool is_occupied_in_adj_nodes = occupied_node_ids_in_adj_list.find(node_candidate_index) != occupied_node_ids_in_adj_list.end();
				bool is_occupied_by_frequent_pattern = nodes_occupied_by_antecedent.find(node_candidate_index) != nodes_occupied_by_antecedent.end();
				if (!adjacency_nodes_are_used[node_candidate_index] && !is_occupied_in_adj_nodes && !is_occupied_by_frequent_pattern)
				{
					// if it is same as that isolated vertex:
					// (we must take the antecedent info of both nodes)
					bool are_same = pu.getAntecedentLabel(adj_list_nodes[node_candidate_index][ADJ_NODES_LABEL]) == pu.getAntecedentLabel(antecedent_pattern_isolated_node_labels[isolated_vertex_current_index]) &&
						pu.getAntecedentChangetime(adj_list_nodes[node_candidate_index][ADJ_NODES_CHANGETIME]) == pu.getAntecedentChangetime(antecedent_pattern_isolated_node_changetime[isolated_vertex_current_index]);

					if (are_same)
					{
						should_backtrack = false;

						which_indices_were_used[isolated_vertex_current_index] = node_candidate_index + 1;
						which_indices_were_used.push_back(0);
						selected_ids_from_adj.push_back(adj_list_nodes[node_candidate_index][ADJ_NODES_ID]);
						adjacency_nodes_are_used[node_candidate_index] = true;
						isolated_vertex_current_index++;
						break;
					}
				}
			}

			// if we could not map this isolated vertex, backtrack:
			if (isolated_vertex_current_index != 0 && should_backtrack)
			{
				which_indices_were_used.pop_back();

				adjacency_nodes_are_used[which_indices_were_used.back() - 1] = false;

				selected_ids_from_adj.pop_back();

				isolated_vertex_current_index--;
			}
			if (isolated_vertex_current_index == 0 && should_backtrack)
			{
				// we tried all combinations, just end:
				break;
			}
		}
	}


	const std::vector<std::vector<int>> combinations(
		const std::vector<int>& input,
		size_t combination_size
	) {
	  	std::vector<std::vector<int>> results;
	  	std::vector<bool> v(input.size());
	  	std::fill(v.begin(), v.begin() + combination_size, true);

	  	do {
			std::vector<int> result;
			for (int i = 0; i < input.size(); ++i) {
		  		if (v[i]) {
					result.push_back(input[i]);
				}
		  	}
			results.push_back(result);
		} while (std::prev_permutation(v.begin(), v.end()));

	  	return results;
	}


	void map_isolated_nodes(
		std::vector<std::vector<int>> &results,
		const std::multiset<node_label_changetime>& isolated_nodes_counts,
		const std::map<node_label_changetime, std::vector<int>>::iterator& occurrences_iterator,
		const std::map<node_label_changetime, std::vector<int>>::iterator& end_occurrences_iterator,
		std::vector<int>& tmp
	) {
	  	//check whether end of mapped node labels was reached and backtrack
	  	if (occurrences_iterator == end_occurrences_iterator) {
			results.push_back(tmp);
			return;
		}

	  	// list all possible combinations of real ids
	  	const std::vector<std::vector<int>> all_combinations = combinations(
	  		occurrences_iterator->second,
			isolated_nodes_counts.count(occurrences_iterator->first)
		);

	  	// move to next item mapped node labels
	  	for (auto const &combination : all_combinations) {
			tmp.insert(tmp.end(), combination.cbegin(), combination.cend());

		  	map_isolated_nodes(
				results,
				isolated_nodes_counts,
				std::next(occurrences_iterator),
				end_occurrences_iterator,
				tmp
			);

			tmp.resize(tmp.size() - combination.size());
		}
	}


	std::vector<int> enumerate_antecedent(std::vector<AdjacencyListCrate> &adjacency_lists,
										  std::vector<int> &graph_ids,
										  std::vector<std::array<int, PAT___SIZE>> &antecedent_pattern_edges,
										  std::vector<std::array<int, PAT___SIZE>> &antecedent_pattern_edges_ADDED,
										  std::vector<int> &antecedent_pattern_node_id_ADDED,
										  std::vector<int> &antecedent_pattern_node_labels_ADDED,
										  std::vector<int> &antecedent_pattern_isolated_node_id,
										  std::vector<int> &antecedent_pattern_isolated_node_labels,
										  std::vector<int> &antecedent_pattern_isolated_node_changetime,
										  PartialUnion pu, bool debug, bool search_for_anomalies,
										  std::vector<std::set<int>> &nodes_occupied_by_antecedent,
										  std::vector<std::set<int>> &edges_occupied_by_antecedent,
										  std::set<anomaly_pattern_with_occurrences> &anomaly_patterns,
										  std::unordered_map<int, std::set<std::set<int>>> &multiple_antecedent_occurrences,
										  bool new_measures)
	{

		std::vector<int> antecedent_occurrences;

		// this is used for enumerating antecedent if not searching for anomalies:
		// for each label/changetime pair, there is a number of occurrences of such a pair
		std::multiset<node_label_changetime> isolated_nodes_counts;
		for (size_t i = 0; i < antecedent_pattern_isolated_node_labels.size(); i++)
		{
			node_label_changetime nlc;
			nlc.label = pu.getAntecedentLabel(antecedent_pattern_isolated_node_labels[i]); // label
			nlc.changetime = pu.getAntecedentChangetime(antecedent_pattern_isolated_node_changetime[i]); // changetime
			isolated_nodes_counts.insert(nlc);
		}


		//because we may have deleted some edges from the original edge_list, this number may be overestimated, but it is not a problem
//		int number_of_nodes_in_edge_list = 0;
//		for (size_t i = 0; i < antecedent_pattern_edges.size(); i++)
//		{
//			if (number_of_nodes_in_edge_list < antecedent_pattern_edges[i][1]) {
//				number_of_nodes_in_edge_list = antecedent_pattern_edges[i][1];
//			}
//			if (number_of_nodes_in_edge_list < antecedent_pattern_edges[i][2]) {
//				number_of_nodes_in_edge_list = antecedent_pattern_edges[i][2];
//			}
//		}
//		number_of_nodes_in_edge_list++;

		for (int g_id : graph_ids)
		{
			// if there are no edges in the antecedent pattern
			if (antecedent_pattern_edges.size() == 0)
			{
				// try to find only the isolated vertices:
				std::multiset<node_label_changetime> adj_list_nodes_count;
			  	std::map<node_label_changetime, std::vector<int>> occurrences_map;
				for (size_t i = 0; i < adjacency_lists[g_id].nodes.size(); i++)
				{
					bool is_vertex_occcupied = nodes_occupied_by_antecedent[g_id].find(i) != nodes_occupied_by_antecedent[g_id].end();
					if (!is_vertex_occcupied)
					{
						node_label_changetime nlc;
						nlc.label = pu.getAntecedentLabel(adjacency_lists[g_id].nodes[i][ADJ_NODES_LABEL]); // label
						nlc.changetime = pu.getAntecedentChangetime(adjacency_lists[g_id].nodes[i][ADJ_NODES_CHANGETIME]); // changetime
						adj_list_nodes_count.insert(nlc);

						// mapping of real ids to nlc
					  	occurrences_map[nlc].push_back(i);
					}
				}

				bool contains_isolated_vertices = true;
				for (auto it = isolated_nodes_counts.begin(); it != isolated_nodes_counts.end(); ++it)
				{
					// for each isolated vertex (i.e. label/changetime pair), check how many times it was found
					// if there are less separate occurrences (those that do not overlap with pattern) that the
					if (adj_list_nodes_count.count(*it) < isolated_nodes_counts.count(*it))
					{
						contains_isolated_vertices = false;
						break;
					}
				}
				if (contains_isolated_vertices)
				{
					// we found the isolated vertices:
					antecedent_occurrences.push_back(g_id);

					if (new_measures) {
						std::vector<std::vector<int>> results;
						std::vector<int> tmp;

						// find all possible mappings of nodes
						map_isolated_nodes(results, isolated_nodes_counts, occurrences_map.begin(), occurrences_map.end(), tmp);

						for (auto const &result : results) {
							std::set<int> antecedent_occurrence(result.begin(), result.end());
							multiple_antecedent_occurrences[g_id].insert(antecedent_occurrence);
						}
					}

					if (search_for_anomalies)
					{
						// if we are searching for anomalies, the procedure is more complicated - we are interested in the positions of the isolated vertices:

						std::set<int> occupied_node_ids_in_adj_list;
						std::vector<std::array<int, PAT___SIZE>> anomaly_pattern_edges;

						find_isolated_nodes_and_addition_edges(adjacency_lists[g_id], antecedent_pattern_edges_ADDED, antecedent_pattern_node_id_ADDED,
							antecedent_pattern_node_labels_ADDED, antecedent_pattern_isolated_node_id,
							antecedent_pattern_isolated_node_labels, antecedent_pattern_isolated_node_changetime, pu, search_for_anomalies,
							nodes_occupied_by_antecedent[g_id], edges_occupied_by_antecedent[g_id], occupied_node_ids_in_adj_list,
							g_id,
							anomaly_pattern_edges, anomaly_patterns, debug);

					}
				}
				// because there are no edges in the antecedent pattern, just move to another graph:
				continue;
			}

			// if we have also edges, do this part:

			// whether we found an occurrence in this graphs:
			bool occurrence_found = false;

			// current adjacency list :
			std::vector<std::vector<int>> adj_list = adjacency_lists[g_id].adjacencyList;
			std::vector<std::vector<std::array<int, ADJ_INFO___SIZE>>> adj_more_info = adjacency_lists[g_id].adjacencyEdgeInfo;

			int current_index = 0;

			// find occurrences in the current adj_list:
			for (size_t u = 0; u < adj_list.size(); u++)
			{
				for (size_t v = 0; v < adj_list[u].size(); v++)
				{
					// NEW POSITION OF THOSE vectors
					// here we have the real IDs of vertices 0, 1, 2, ... from the pattern
					std::vector<int> sequence_of_nodes;

					std::vector<int> sequence_of_used_nodes;  // this vector will have even length - it will contain id's (by pairs) of nodes of each occupied edge (i.e. node ids from pattern)

					// these two vectors save src and dst vertices (their IDs) which correspond to real occurrences of the src and dst vertices of the pattern
					std::vector<int> real_ids_src(antecedent_pattern_edges.size(), 0);
					std::vector<int> real_ids_dst(antecedent_pattern_edges.size(), 0);
					std::vector<int> real_ids_dst_ind(antecedent_pattern_edges.size(), 0);
					std::vector<int> real_ids_src_ind(antecedent_pattern_edges.size(), 0);

					// begin by going through all edges in the adjacency list of the current graph:

					// is the SRC-vertex occupied by the antecedent of the frequent pattern?
					bool is_src_vertex_occcupied = nodes_occupied_by_antecedent[g_id].find(u) != nodes_occupied_by_antecedent[g_id].end();
					// is the DST-vertex occupied by the antecedent of the frequent pattern?
					bool is_dst_vertex_occcupied = nodes_occupied_by_antecedent[g_id].find(adj_list[u][v]) != nodes_occupied_by_antecedent[g_id].end();
					// is the edge itself occupied by the antecedent of the frequent pattern?
					bool is_edge_occcupied = edges_occupied_by_antecedent[g_id].find(adj_more_info[u][v][ADJ_INFO_ID]) != edges_occupied_by_antecedent[g_id].end();

					bool it_matches = pu.matches_the_adj_info_to_antecedent_pattern_edge(adj_more_info[u][v], antecedent_pattern_edges[current_index], true);

					// if nothing from this adjacency-list edge is occupied, and it matches the antecedent-pattern edge, use it:
					if (!is_src_vertex_occcupied && !is_dst_vertex_occcupied && !is_edge_occcupied && it_matches)
					{

						sequence_of_used_nodes.push_back(antecedent_pattern_edges[current_index][0]);
						sequence_of_used_nodes.push_back(antecedent_pattern_edges[current_index][1]);
						int id_src = u;
						int id_dst = adj_list[u][v];

						real_ids_src[0] = id_src;
						real_ids_dst[0] = id_dst;
						real_ids_dst_ind[0] = v;
						for (size_t i = 0; i < adj_list[id_dst].size(); i++)
						{
							if (adj_list[id_dst][i] == id_src && pu.matches_the_adj_info_to_antecedent_pattern_edge(flipAdjacencyInfo(adj_more_info[id_dst][i]), antecedent_pattern_edges[current_index], true))
							{
								real_ids_src_ind[0] = i;
								break;
							}
						}

						// here we save src_indices and dst_indices which should be starting points when searching for the i-th edge in edge_list
						// at the beginning, it's full of 0's. When an edge is found, appropriate indices are filled in.
						// if backtracking is used, then we use the last value on the specified index.
						// sequence_of_edge_nodesids is used only when we search through the whole adjacency list - here we save the lastly used src_vertex id
						// sequence_of_edge_indices is used in all 4 cases - here we save the lastly used index (either src_index or dst_index) for the given vertex_id
						std::vector<int> sequence_of_edge_nodesids(antecedent_pattern_edges.size(), 0);
						std::vector<int> sequence_of_edge_indices(antecedent_pattern_edges.size(), 0);
						std::vector<int> sequence_of_edge_nodesids_is_flipped(antecedent_pattern_edges.size(), false); // this is used when we want to get the final anomaly pattern

						sequence_of_edge_nodesids[current_index] = u; // new
						sequence_of_edge_indices[current_index] = v + 1; // new
						sequence_of_edge_nodesids_is_flipped[current_index] = false;

						current_index++;

						// do DFS
						while (true)
						{
							// if we found the occurrence of the whole pattern:
							if (current_index >= antecedent_pattern_edges.size())
							{
							  	// now try to map the isolated vertices:
								std::multiset<node_label_changetime> adj_list_nodes_count;
							  	std::map<node_label_changetime, std::vector<int>> occurrences_map;
								for (size_t i = 0; i < adjacency_lists[g_id].nodes.size(); i++)
								{
									bool is_vertex_occcupied = nodes_occupied_by_antecedent[g_id].find(i) != nodes_occupied_by_antecedent[g_id].end();

									// if ID (index 0) of the node adjacency_lists[g_id].nodes[i] is not in sequence_of_used_nodes:
									if (!is_vertex_occcupied && std::find(sequence_of_used_nodes.begin(), sequence_of_used_nodes.end(), adjacency_lists[g_id].nodes[i][0]) == sequence_of_used_nodes.end())
									{
										node_label_changetime nlc;
										nlc.label = pu.getAntecedentLabel(adjacency_lists[g_id].nodes[i][ADJ_NODES_LABEL]); // label
										nlc.changetime = pu.getAntecedentChangetime(adjacency_lists[g_id].nodes[i][ADJ_NODES_CHANGETIME]); // changetime
										adj_list_nodes_count.insert(nlc);

									  	// mapping of real ids to nlc
									  	occurrences_map[nlc].push_back(i);
									}
								}


								bool contains_isolated_vertices = true;
								for (std::multiset<node_label_changetime>::iterator it = isolated_nodes_counts.begin(); it != isolated_nodes_counts.end(); ++it)
								{
									if (adj_list_nodes_count.count(*it) < isolated_nodes_counts.count(*it))
									{
										contains_isolated_vertices = false;
										break;
									}
								}

								if (contains_isolated_vertices)
								{
									// we found also the isolated vertices:
									antecedent_occurrences.push_back(g_id);
									// this variable is used for breaking the outer loops:
									occurrence_found = true;

									if (new_measures) {
									  	std::set<int> occurrence_vertexes;
									  	for (size_t i = 0; i < antecedent_pattern_edges.size(); i++) {
										  	occurrence_vertexes.insert(real_ids_src[i]);
										  	occurrence_vertexes.insert(real_ids_dst[i]);
										}

									  	std::vector<std::vector<int>> results;
									  	std::vector<int> tmp;

									  	// find all possible mappings of nodes
									  	map_isolated_nodes(results, isolated_nodes_counts, occurrences_map.begin(), occurrences_map.end(), tmp);

									  	for (auto const &result : results) {
											std::set<int> antecedent_occurrence(result.begin(), result.end());
										  	antecedent_occurrence.insert(occurrence_vertexes.begin(), occurrence_vertexes.end());
											multiple_antecedent_occurrences[g_id]insert(antecedent_occurrence);
										}
									}


									if (search_for_anomalies)
									{
										// if we are searching for anomalies, the procedure is more complicated - we are interested in the positions of the isolated vertices:

									  	std::set<int> occupied_node_ids_in_adj_list;
									  	for (size_t i = 0; i < antecedent_pattern_edges.size(); i++) {
											occupied_node_ids_in_adj_list.insert(real_ids_src[i]);
											occupied_node_ids_in_adj_list.insert(real_ids_dst[i]);
										}

										// also, get the anomaly-pattern edges
										std::vector<std::array<int, PAT___SIZE>> anomaly_pattern_edges;
										// fill it by using sequence_of_edge_nodesids and sequence_of_edge_indices

                                        // FOUND MAPPING OF ANTECEDENT REGULAR EDGES
										for (size_t i = 0; i < antecedent_pattern_edges.size(); i++)
										{

											std::array<int, PAT___SIZE> anomaly_edge;
											anomaly_edge[PAT_SRC_ID] = antecedent_pattern_edges[i][PAT_SRC_ID];
											anomaly_edge[PAT_DST_ID] = antecedent_pattern_edges[i][PAT_DST_ID];

											std::array<int, ADJ_INFO___SIZE> loc_adj_info = adj_more_info[sequence_of_edge_nodesids[i]][sequence_of_edge_indices[i] - 1];

											if (sequence_of_edge_nodesids_is_flipped[i])
											{
												loc_adj_info = flipAdjacencyInfo(loc_adj_info);
											}
											anomaly_edge[PAT_SRC_LABEL] = loc_adj_info[ADJ_INFO_SRC_LABEL];
											anomaly_edge[PAT_SRC_CHANGETIME] = loc_adj_info[ADJ_INFO_SRC_CHANGETIME];
											anomaly_edge[PAT_DIRECTION] = loc_adj_info[ADJ_INFO_DIRECTION];
											anomaly_edge[PAT_LABEL] = loc_adj_info[ADJ_INFO_LABEL];
											anomaly_edge[PAT_CHANGETIME] = loc_adj_info[ADJ_INFO_CHANGETIME];
											anomaly_edge[PAT_DST_LABEL] = loc_adj_info[ADJ_INFO_DST_LABEL];
											anomaly_edge[PAT_DST_CHANGETIME] = loc_adj_info[ADJ_INFO_DST_CHANGETIME];

											anomaly_edge[PAT_ID] = antecedent_pattern_edges[i][PAT_ID];

											


											anomaly_pattern_edges.push_back(anomaly_edge);
										}

										find_isolated_nodes_and_addition_edges(adjacency_lists[g_id], antecedent_pattern_edges_ADDED, antecedent_pattern_node_id_ADDED,
											antecedent_pattern_node_labels_ADDED, antecedent_pattern_isolated_node_id,
											antecedent_pattern_isolated_node_labels, antecedent_pattern_isolated_node_changetime, pu, search_for_anomalies,
											nodes_occupied_by_antecedent[g_id], edges_occupied_by_antecedent[g_id], occupied_node_ids_in_adj_list,
											g_id,
											anomaly_pattern_edges, anomaly_patterns, debug);
									}
								}


								// if we didn't find appropriate isolated vertices, backtrack:
								// we didn't found any appropriate edge candidate, so mark it and backtrack
								if (current_index < sequence_of_edge_indices.size())
								{
									sequence_of_edge_nodesids[current_index] = 0;
									sequence_of_edge_indices[current_index] = 0;
									sequence_of_edge_nodesids_is_flipped[current_index] = false;
								}

								// remove the last two elements from the vector:
								sequence_of_used_nodes.pop_back();
								sequence_of_used_nodes.pop_back();
								// we don't need to clean the new_real_ids and new_real_ids_ind ids
								current_index--;


							} // (if we found the occurrence of the whole pattern)


							if (current_index == 0)
							{
								break;
							}

							// traverse:
							int i = antecedent_pattern_edges[current_index][0];
							int j = antecedent_pattern_edges[current_index][1];

							int i_real_id = -1;
							int j_real_id = -1;
							for (size_t si = 0; si < sequence_of_used_nodes.size(); si++)
							{
								if (sequence_of_used_nodes[si] == i)
								{
									if (si % 2 == 0)
									{
										i_real_id = real_ids_src[si / 2];
									}
									else
									{
										i_real_id = real_ids_dst[(si - 1) / 2];
									}
								}
								if (sequence_of_used_nodes[si] == j)
								{
									if (si % 2 == 0)
									{
										j_real_id = real_ids_src[si / 2];
									}
									else
									{
										j_real_id = real_ids_dst[(si - 1) / 2];
									}
								}
							}

							sequence_of_used_nodes.push_back(i);
							sequence_of_used_nodes.push_back(j);

							// traverse the adjacency list:
							bool should_backtrack = true;


							if (i_real_id == -1)
							{
								if (j_real_id == -1)
								{

									// we have to go through the whole adjacency list and find a fit
									if (sequence_of_edge_indices[current_index] >= adj_list[sequence_of_edge_nodesids[current_index]].size())
									{
										// if we went through the whole vector of neighbours of the considered vertex, then we should move to another vertex
										sequence_of_edge_indices[current_index] = 0;
										sequence_of_edge_nodesids[current_index] = sequence_of_edge_nodesids[current_index] + 1;
										sequence_of_edge_nodesids_is_flipped[current_index] = false;
									}

									for (int src_candidate = sequence_of_edge_nodesids[current_index]; src_candidate < adj_list.size(); src_candidate++)
									{
										// if we are doing the starting node, then we should continue from the last position,
										// otherwise (we advanced to other nodes in the outer loop, and we should start from the beginning):
										int starting_point = (src_candidate == sequence_of_edge_nodesids[current_index]) ? sequence_of_edge_indices[current_index] : 0;

										for (int src_index = starting_point; src_index < adj_list[src_candidate].size(); src_index++)
										{
											// is the SRC-vertex occupied by the antecedent of the frequent pattern?
											bool local_is_src_vertex_occcupied = nodes_occupied_by_antecedent[g_id].find(src_candidate) != nodes_occupied_by_antecedent[g_id].end();
											// is the DST-vertex occupied by the antecedent of the frequent pattern?
											bool local_is_dst_vertex_occcupied = nodes_occupied_by_antecedent[g_id].find(adj_list[src_candidate][src_index]) != nodes_occupied_by_antecedent[g_id].end();
											// is the edge itself occupied by the antecedent of the frequent pattern?
											bool local_is_edge_occcupied = edges_occupied_by_antecedent[g_id].find(adj_more_info[src_candidate][src_index][ADJ_INFO_ID]) != edges_occupied_by_antecedent[g_id].end();

											// begin by going through all edges in the adjacency list of the current graph:
											if (!local_is_src_vertex_occcupied && !local_is_dst_vertex_occcupied && !local_is_edge_occcupied &&
												pu.matches_the_adj_info_to_antecedent_pattern_edge(adj_more_info[src_candidate][src_index], antecedent_pattern_edges[current_index], true))
											{
												bool should_skip_this_edge = false;

												for (size_t k = 0; k < current_index; k++)
												{
													// if we already occupied the edge in the regular direction or the opposite one
													if ((real_ids_src[k] == src_candidate && real_ids_dst_ind[k] == src_index)
														|| (real_ids_dst[k] == src_candidate && real_ids_src_ind[k] == src_index))
													{
														should_skip_this_edge = true;
														break;
													}
												}
												if (!should_skip_this_edge)
												{
													should_backtrack = false;
													id_src = src_candidate;
													id_dst = adj_list[src_candidate][src_index];

													real_ids_src[current_index] = id_src;
													real_ids_dst[current_index] = id_dst;
													real_ids_dst_ind[current_index] = src_index;
													for (size_t k = 0; k < adj_list[id_dst].size(); k++) // what is the occupied index for the "to" vertex
													{
														if (adj_list[id_dst][k] == id_src && pu.matches_the_adj_info_to_antecedent_pattern_edge(flipAdjacencyInfo(adj_more_info[id_dst][k]), antecedent_pattern_edges[current_index], true))
														{
															real_ids_src_ind[current_index] = k;
															break;
														}
													}
													

													sequence_of_edge_nodesids[current_index] = src_candidate;
													sequence_of_edge_indices[current_index] = src_index + 1;
													sequence_of_edge_nodesids_is_flipped[current_index] = false;
													current_index++;
													break;
												}
											}
										}

										if (!should_backtrack)
										{
											// we found the edge position, so break also the outer for loop
											break;
										}
									}
								}
								else
								{

									// we have to use only the specific node denoting the to vertex
									for (size_t dst_index = sequence_of_edge_indices[current_index]; dst_index < adj_list[j_real_id].size(); dst_index++)
									{
										// is the SRC-vertex occupied by the antecedent of the frequent pattern?
										bool local_is_src_vertex_occcupied = nodes_occupied_by_antecedent[g_id].find(j_real_id) != nodes_occupied_by_antecedent[g_id].end();
										// is the DST-vertex occupied by the antecedent of the frequent pattern?
										bool local_is_dst_vertex_occcupied = nodes_occupied_by_antecedent[g_id].find(adj_list[j_real_id][dst_index]) != nodes_occupied_by_antecedent[g_id].end();
										// is the edge itself occupied by the antecedent of the frequent pattern?
										bool local_is_edge_occcupied = edges_occupied_by_antecedent[g_id].find(adj_more_info[j_real_id][dst_index][ADJ_INFO_ID]) != edges_occupied_by_antecedent[g_id].end();

										if (!local_is_src_vertex_occcupied && !local_is_dst_vertex_occcupied && !local_is_edge_occcupied &&
											pu.matches_the_adj_info_to_antecedent_pattern_edge(flipAdjacencyInfo(adj_more_info[j_real_id][dst_index]), antecedent_pattern_edges[current_index], true))
										{
											bool should_skip_this_edge = false;

											for (size_t k = 0; k < current_index; k++)
											{
												// if we already occupied the edge in the regular direction or the opposite one
												if ((real_ids_src[k] == j_real_id && real_ids_dst_ind[k] == dst_index)
													|| (real_ids_dst[k] == j_real_id && real_ids_src_ind[k] == dst_index))
												{
													should_skip_this_edge = true;
													break;
												}
											}

											if (!should_skip_this_edge)
											{
												should_backtrack = false;
												id_src = j_real_id;
												id_dst = adj_list[j_real_id][dst_index];

												real_ids_src[current_index] = id_src;
												real_ids_dst[current_index] = id_dst;
												real_ids_dst_ind[current_index] = dst_index;
												for (size_t k = 0; k < adj_list[id_dst].size(); k++) // what is the occupied index for the "to" vertex
												{
													if (adj_list[id_dst][k] == id_src && pu.matches_the_adj_info_to_antecedent_pattern_edge(flipAdjacencyInfo(adj_more_info[id_dst][k]), antecedent_pattern_edges[current_index], true))
													{
														real_ids_src_ind[current_index] = k;
														break;
													}
												}

												sequence_of_edge_nodesids[current_index] = j_real_id;
												sequence_of_edge_indices[current_index] = dst_index + 1;
												sequence_of_edge_nodesids_is_flipped[current_index] = true;

												current_index++;
												break;
											}

										}

									}
								}
							}
							else
							{
								if (j_real_id != -1)
								{

									// we have to use the specific nodes denoting the src and dst vertices
									for (int src_index = sequence_of_edge_indices[current_index]; src_index < adj_list[i_real_id].size(); src_index++)
									{
										if (adj_list[i_real_id][src_index] == j_real_id)
										{
											// if we found a position of the "to" vertex

											// is the SRC-vertex occupied by the antecedent of the frequent pattern?
											bool local_is_src_vertex_occcupied = nodes_occupied_by_antecedent[g_id].find(i_real_id) != nodes_occupied_by_antecedent[g_id].end();
											// is the DST-vertex occupied by the antecedent of the frequent pattern?
											bool local_is_dst_vertex_occcupied = nodes_occupied_by_antecedent[g_id].find(adj_list[i_real_id][src_index]) != nodes_occupied_by_antecedent[g_id].end();
											// is the edge itself occupied by the antecedent of the frequent pattern?
											bool local_is_edge_occcupied = edges_occupied_by_antecedent[g_id].find(adj_more_info[i_real_id][src_index][ADJ_INFO_ID]) != edges_occupied_by_antecedent[g_id].end();

											if (!local_is_src_vertex_occcupied && !local_is_dst_vertex_occcupied && !local_is_edge_occcupied &&
												pu.matches_the_adj_info_to_antecedent_pattern_edge(adj_more_info[i_real_id][src_index], antecedent_pattern_edges[current_index], true))
											{
												// and if we found the right edge:
												bool should_skip_this_edge = false;

												for (size_t k = 0; k < current_index; k++)
												{
													// if we already occupied the edge in the regular direction or the opposite one
													if ((real_ids_src[k] == i_real_id && real_ids_dst_ind[k] == src_index)
														|| (real_ids_dst[k] == i_real_id && real_ids_src_ind[k] == src_index))
													{
														should_skip_this_edge = true;
														break;
													}
												}

												if (!should_skip_this_edge)
												{
													should_backtrack = false;
													id_src = i_real_id;
													id_dst = j_real_id;

													real_ids_src[current_index] = id_src;
													real_ids_dst[current_index] = id_dst;
													real_ids_dst_ind[current_index] = src_index;
													for (size_t k = 0; k < adj_list[id_dst].size(); k++) // what is the occupied index for the "to" vertex
													{
														if (adj_list[id_dst][k] == id_src && pu.matches_the_adj_info_to_antecedent_pattern_edge(flipAdjacencyInfo(adj_more_info[id_dst][k]), antecedent_pattern_edges[current_index], true))
														{
															real_ids_src_ind[current_index] = k;
															break;
														}
													}

													sequence_of_edge_nodesids[current_index] = i_real_id;
													sequence_of_edge_indices[current_index] = src_index + 1;
													sequence_of_edge_nodesids_is_flipped[current_index] = false;

													current_index++;
													break;
												}

											}
										}
									}
								}
								else
								{

									// we have to use only the specific node denoting the src vertex
									for (int src_index = sequence_of_edge_indices[current_index]; src_index < adj_list[i_real_id].size(); src_index++)
									{
										// is the SRC-vertex occupied by the antecedent of the frequent pattern?
										bool local_is_src_vertex_occcupied = nodes_occupied_by_antecedent[g_id].find(i_real_id) != nodes_occupied_by_antecedent[g_id].end();
										// is the DST-vertex occupied by the antecedent of the frequent pattern?
										bool local_is_dst_vertex_occcupied = nodes_occupied_by_antecedent[g_id].find(adj_list[i_real_id][src_index]) != nodes_occupied_by_antecedent[g_id].end();
										// is the edge itself occupied by the antecedent of the frequent pattern?
										bool local_is_edge_occcupied = edges_occupied_by_antecedent[g_id].find(adj_more_info[i_real_id][src_index][ADJ_INFO_ID]) != edges_occupied_by_antecedent[g_id].end();

										if (!local_is_src_vertex_occcupied && !local_is_dst_vertex_occcupied && !local_is_edge_occcupied &&
											pu.matches_the_adj_info_to_antecedent_pattern_edge(adj_more_info[i_real_id][src_index], antecedent_pattern_edges[current_index], true))
										{
											bool should_skip_this_edge = false;

											for (size_t k = 0; k < current_index; k++)
											{
												// if we already occupied the edge in the regular direction or the opposite one
												if ((real_ids_src[k] == i_real_id && real_ids_dst_ind[k] == src_index)
													|| (real_ids_dst[k] == i_real_id && real_ids_src_ind[k] == src_index))
												{
													should_skip_this_edge = true;
													break;
												}
											}

											if (!should_skip_this_edge)
											{
												should_backtrack = false;
												id_src = i_real_id;
												id_dst = adj_list[i_real_id][src_index];

												real_ids_src[current_index] = id_src;
												real_ids_dst[current_index] = id_dst;
												real_ids_dst_ind[current_index] = src_index;
												for (size_t k = 0; k < adj_list[id_dst].size(); k++) // what is the occupied index for the "to" vertex
												{
													if (adj_list[id_dst][k] == id_src && pu.matches_the_adj_info_to_antecedent_pattern_edge(flipAdjacencyInfo(adj_more_info[id_dst][k]), antecedent_pattern_edges[current_index], true))
													{
														real_ids_src_ind[current_index] = k;
														break;
													}
												}


												sequence_of_edge_nodesids[current_index] = i_real_id;
												sequence_of_edge_indices[current_index] = src_index + 1;
												sequence_of_edge_nodesids_is_flipped[current_index] = false;

												current_index++;
												break;
											}
										}
									}
								}
							}


							if (should_backtrack)
							{

								// we didn't found any appropriate edge candidate, so mark it and backtrack
								if (current_index < sequence_of_edge_indices.size())
								{
									sequence_of_edge_nodesids[current_index] = 0;
									sequence_of_edge_indices[current_index] = 0;
									sequence_of_edge_nodesids_is_flipped[current_index] = false;
								}
								// remove the last two elements from the vector:
								sequence_of_used_nodes.pop_back();
								sequence_of_used_nodes.pop_back();
								// we don't need to clean the new_real_ids and new_real_ids_ind ids
								current_index--;
							}
						}

						if (occurrence_found && !search_for_anomalies)
						{

							break;
						}

					}
				}

				if (occurrence_found && !search_for_anomalies)
				{

					break;
				}
			}
		}

		return antecedent_occurrences;
	}


}
