
#include "FileReader.h"
#include "DGRMiner.h"
#include "DGRSubgraphMining.h"
#include "HelperPrintFunctions.h"

#include <fstream> // temp
#include <sstream>
#include <algorithm>
#include <cmath>



namespace dgrminer
{

	void print_adjacency_lists(std::vector<AdjacencyListCrate> adjacency_lists)
	{
		for (size_t i = 0; i < adjacency_lists.size(); i++)
		{

			cout << "ADJ NODES: " << i << endl;
			for (size_t j = 0; j < adjacency_lists[i].nodes.size(); j++)
			{
				cout << adjacency_lists[i].nodes[j][ADJ_NODES_ID] << ", "
					 << adjacency_lists[i].nodes[j][ADJ_NODES_CHANGETIME] << ", "
					 << adjacency_lists[i].nodes[j][ADJ_NODES_LABEL] << endl;
			}

			cout << "ADJ LIST: " << i << endl;
			// print adj list:
			for (size_t j = 0; j < adjacency_lists[i].adjacencyList.size(); j++)
			{
				cout << j << ": ";
				for (size_t k = 0; k < adjacency_lists[i].adjacencyList[j].size(); k++)
				{
					cout << adjacency_lists[i].adjacencyList[j][k] << " ";
				}
				cout << endl;
			}

			cout << "MORE INFO LIST: " << i << endl;
			// print more info:
			for (size_t j = 0; j < adjacency_lists[i].adjacencyEdgeInfo.size(); j++)
			{
				cout << j << ": " << endl;;
				for (size_t k = 0; k < adjacency_lists[i].adjacencyEdgeInfo[j].size(); k++)
				{
					for (size_t l = 0; l < ADJ_INFO___SIZE; l++)
					{
						cout << adjacency_lists[i].adjacencyEdgeInfo[j][k][l] << ", ";
					}
					cout << endl;

				}
				cout << endl;
			}
		}
	}


	void run_DGRMiner(std::string input_file, std::string output_file, double min_support, double min_confidence,
					  bool compute_confidence, int window_size, std::string str_timeabstraction,
					  bool search_for_anomalies, double min_anomaly_outlierness, bool verbose)
	{
		// set abstractions appropriately
		bool simple_time_abstraction = (str_timeabstraction == "bin_all");
		bool simple_node_time_abstraction = (str_timeabstraction == "bin_nodes");

		// save result patterns in the results_crate
		results_crate results;
		results_crate_anomalies results_anomalies;
		results.saved_instances = 0;
		results_anomalies.anomaly_saved_instances = 0;

		bool debugPrint = true;


		FileReader fr;
		PartialUnion pu;

		debug_println(verbose, "--- STARTING DGRMINER");
		debug_print(verbose, "--- READING FILE");
		pu = fr.read_dynamic_graph_from_file(input_file, window_size, search_for_anomalies);
		debug_println(verbose, "(DONE)");
		pu.printDimensions();
		debug_println(verbose, "Number of dynamic graphs: ", pu.getNumberOfDynamicGraphs());


		bool set_of_graphs = pu.getNumberOfDynamicGraphs() > 1;


		if (pu.getNumberOfNodes() == 0)
		{
			debug_println(verbose, "The union graph has no nodes! Quitting...");
			return;
		}

		int snapshots = pu.getNumberOfSnapshots();
		int max_absolute_support = snapshots;
		if (set_of_graphs)
		{
			max_absolute_support = pu.queryMappingSnapshotsToGraphs(snapshots - 1) + 1;
		}
		// what is the minimum support expressed by an absolute value
		int min_absolute_support = (int) ceil(max_absolute_support * min_support);

		debug_println(verbose, "Max absolute support: ", max_absolute_support);
		debug_println(verbose, "Number of snapshots: ", snapshots);
		debug_println(verbose, "Min support (absolute): ", min_absolute_support);

		if (simple_time_abstraction)
		{
			pu.performSimpleTimeAbstraction();
		}
		else if (simple_node_time_abstraction)
		{
			pu.performSimpleNodeTimeAbstraction();
		}

		// we cannot remove infrequent vertices and edges if we are computing confidence (= if we are computing antecedents!!!! - also when computing anomalies)
		if (!compute_confidence)
		{
			debug_println(verbose, "--- REMOVING INFREQUENT VERTICES");
		}

		// compute the frequent vertices and remove the infrequent ones,
        // save the frequent ones into results
		pu.computeFrequentVerticesAndRemoveTheInfrequentOnes(min_absolute_support, min_confidence, compute_confidence,
															 &results, &results_anomalies, max_absolute_support,
															 set_of_graphs,
															 search_for_anomalies, min_anomaly_outlierness);
		if (!compute_confidence)
		{
			debug_println(verbose, "--- (DONE)");
			debug_println(verbose, "--- REMOVING INVALID EDGES");
		}


		std::set<labeled_node_time> labeled_nodes = pu.compute_labeled_nodes();
		if (!compute_confidence)
		{
			pu.eraseInvalidEdges(labeled_nodes);
			debug_println(verbose, "--- (DONE)");

			// remove infrequent edges
			debug_println(verbose, "--- REMOVING INFREQUENT EDGES");
		}

		std::vector<std::array<int, 8>> newedges;  // will be used for adjacency info
		std::set<labeled_edge_with_occurrences> edges_set; // this contains edges flipped to be smaller, used for counting support of the edges
		pu.compute_labeled_edges(newedges, edges_set, labeled_nodes, debugPrint); // computes newedges, edges_set

		if (!compute_confidence)
		{
			pu.removeInfrequentEdges(newedges, edges_set, min_absolute_support, set_of_graphs, debugPrint);
			debug_println(verbose, "--- (DONE)");
		}

		// build adjacency lists (one list for each snapshot):
		debug_print(verbose, "--- BUILDING ADJACENCY LISTS ");
		std::vector<AdjacencyListCrate> adjacency_lists = pu.createAdjacencyLists(newedges, snapshots);
		debug_println(verbose, "(DONE)");


		// create initial patterns:
		debug_print(verbose, "--- GETTING INITIAL PATTERNS ");
		std::vector<std::array<int, 8>> initial_patterns;
		std::vector<std::vector<int>> initial_patterns_occurrences;
		getFrequentInitialPatterns(edges_set, initial_patterns, initial_patterns_occurrences, min_absolute_support,
								   set_of_graphs, pu); // fills initial_patterns, initial_patterns_occurrences
		debug_println(verbose, "(DONE)");


		vector<int> antecedent_graph_ids;
		for (int i = 0; i < snapshots; i++)
		{
			antecedent_graph_ids.push_back(i);

		}

		debug_println(verbose, "--- FINDING FREQUENT PATTERNS (STARTING FROM ", initial_patterns.size(),
					  " INITIAL PATTERNS):");
		debug_println(verbose, "--- (", results.result_nodes.size(), " SINGLE-VERTEX PATTERNS ALREADY FOUND)");

		printResultsToFiles(&results, &results_anomalies, pu, output_file, set_of_graphs, compute_confidence,
							search_for_anomalies, false);


		for (size_t i = 0; i < initial_patterns.size(); i++)
		{
			std::vector<std::array<int, 8>> starting_edges;
			for (size_t j = 0; j <= i; j++)
			{
				starting_edges.push_back(initial_patterns[j]);
			}
			std::vector<std::array<int, 10>> init_pattern;
			std::array<int, 10> i_p = {
					0,
					1,
					initial_patterns[i][0],
					initial_patterns[i][1],
					initial_patterns[i][2],
					initial_patterns[i][3],
					initial_patterns[i][4],
					initial_patterns[i][5],
					initial_patterns[i][6],
					initial_patterns[i][7]
			};

			init_pattern.push_back(i_p);

			DGRSubgraphMining(adjacency_lists, initial_patterns_occurrences[i], init_pattern, min_absolute_support,
							  starting_edges, &results, &results_anomalies,
							  max_absolute_support, min_confidence, compute_confidence, pu, antecedent_graph_ids,
							  set_of_graphs, search_for_anomalies, min_anomaly_outlierness, output_file, verbose);
		}

		debug_println(verbose, "FINISHED.");
	}


	void getFrequentInitialPatterns(std::set<labeled_edge_with_occurrences> &edges_set,
									std::vector<std::array<int, 8>> &initial_patterns,
									std::vector<std::vector<int>> &initial_patterns_occurrences,
									int support_as_absolute, bool set_of_graphs, PartialUnion pu)
	{
		for (auto f : edges_set)
		{
			if (set_of_graphs)
			{
				std::set<int> mapped_occurrences;
				std::set<int>::iterator it2;
				for (it2 = f.occurrences.begin(); it2 != f.occurrences.end(); ++it2)
				{
					mapped_occurrences.insert(pu.queryMappingSnapshotsToGraphs(*it2));
				}
				if (static_cast<int>(mapped_occurrences.size()) >= support_as_absolute &&
					(f.elements[ADJ_INFO_SRC_CHANGETIME] >= 0 || f.elements[ADJ_INFO_CHANGETIME] >= 0 ||
					 f.elements[ADJ_INFO_DST_CHANGETIME] >= 0))
				{
					initial_patterns.push_back(f.elements);

					std::vector<int> occs;
					std::copy(f.occurrences.begin(), f.occurrences.end(), std::back_inserter(occs));

					initial_patterns_occurrences.push_back(occs);
				}
			}
			else
			{
				if (static_cast<int>(f.occurrences.size()) >= support_as_absolute &&
					(f.elements[ADJ_INFO_SRC_CHANGETIME] >= 0 || f.elements[ADJ_INFO_CHANGETIME] >= 0 ||
					 f.elements[ADJ_INFO_DST_CHANGETIME] >= 0))
				{
					// if the edge represents at least one change
					initial_patterns.push_back(f.elements);

					std::vector<int> occs;
					std::copy(f.occurrences.begin(), f.occurrences.end(), std::back_inserter(occs));

					initial_patterns_occurrences.push_back(occs);
				}
			}
		}
	}

}