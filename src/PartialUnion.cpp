
#include "HelperFunctions.h"
#include "PartialUnion.h"
#include "AdjacencyList.h"
#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <vector>
#include <algorithm>

namespace dgrminer
{

	void printResultsToFiles(results_crate * results, results_crate_anomalies * results_anomalies, PartialUnion pu, std::string output_file, bool set_of_graphs,
		bool compute_confidence, bool search_for_anomalies, bool append_to_file)
	{
		// NODES
		ofstream myfile;
		if (append_to_file) {
			myfile.open(output_file + "_nodes", std::ios_base::app);
		}
		else {
			myfile.open(output_file + "_nodes");
			myfile << "pattern,id,label_int,changetime" << endl;
		}
		for (size_t i = 0; i < results->result_nodes.size(); i++)
		{
			for (int j = 0; j < 4; j++)
			{
				myfile << results->result_nodes[i][j];
				if (j < 3) {
					myfile << ",";
				}
			}
			myfile << endl;
		}
		myfile.close();

		// EDGES
		if (append_to_file) {
			myfile.open(output_file + "_edges", std::ios_base::app);
		}
		else {
			myfile.open(output_file + "_edges");
			myfile << "pattern,src,dst,label_int,direction,edgetime" << endl;
		}
		for (size_t i = 0; i < results->result_edges.size(); i++)
		{
			for (int j = 0; j < 6; j++)
			{
				myfile << results->result_edges[i][j];
				if (j < 5) {
					myfile << ",";
				}
			}
			myfile << endl;
		}
		myfile.close();

		// MEASURES
		if (append_to_file) {
			myfile.open(output_file + "_measures", std::ios_base::app);
		}
		else {
			myfile.open(output_file + "_measures");
			myfile << "support_absolute,support_relative";
			if (compute_confidence)
			{
				myfile << ",confidence";
			}
			myfile << endl;
		}
		for (size_t i = 0; i < results->support_absolute.size(); i++)
		{
			myfile << results->support_absolute[i];
			myfile << "," << results->support[i];
			if (compute_confidence)
			{
				myfile << "," << results->confidence[i];
			}
			myfile << endl;
		}
		myfile.close();

		// index snapshots from 1

		// OCCURRENCES
		if (append_to_file) {
			myfile.open(output_file + "_occurrences", std::ios_base::app);
		}
		else {
			myfile.open(output_file + "_occurrences");
			if (set_of_graphs)
			{
				myfile << "graphs" << endl;
			}
			else
			{
				myfile << "graph_snapshots" << endl;
			}
		}
		for (size_t i = 0; i < results->occurrences.size(); i++)
		{

			for (int j = 0; j < results->occurrences[i].size(); j++)
			{
				myfile << (results->occurrences[i][j] + 1);
				if (j < results->occurrences[i].size() - 1) {
					myfile << ",";
				}
			}

			myfile << endl;
		}
		myfile.close();



		// print anomalies:
		if (search_for_anomalies)
		{
			// NODES
			ofstream myfile;
			if (append_to_file) {
				myfile.open(output_file + "_anomalies_nodes", std::ios_base::app);
			}
			else {
				myfile.open(output_file + "_anomalies_nodes");
				myfile << "anomaly_pattern,id,label_int,changetime" << endl;
			}
			for (size_t i = 0; i < results_anomalies->anomaly_result_nodes.size(); i++)
			{
				for (int j = 0; j < 4; j++)
				{
					myfile << results_anomalies->anomaly_result_nodes[i][j];
					if (j < 3) {
						myfile << ",";
					}
				}
				myfile << endl;
			}
			myfile.close();

			// EDGES
			if (append_to_file) {
				myfile.open(output_file + "_anomalies_edges", std::ios_base::app);
			}
			else {
				myfile.open(output_file + "_anomalies_edges");
				myfile << "anomaly_pattern,src,dst,label_int,direction,edgetime" << endl;
			}
			for (size_t i = 0; i < results_anomalies->anomaly_result_edges.size(); i++)
			{
				for (int j = 0; j < 6; j++)
				{
					myfile << results_anomalies->anomaly_result_edges[i][j];
					if (j < 5) {
						myfile << ",";
					}
				}
				myfile << endl;
			}
			myfile.close();

			// LINKING TO FREQUENT PATTERNS
			if (append_to_file) {
				myfile.open(output_file + "_anomalies_explanation", std::ios_base::app);
			}
			else {
				myfile.open(output_file + "_anomalies_explanation");
				myfile << "anomaly_pattern,pattern";
				myfile << endl;
			}
			for (size_t i = 0; i < results_anomalies->anomaly_id_of_anomalous_pattern.size(); i++)
			{
				myfile << results_anomalies->anomaly_id_of_anomalous_pattern[i];
				myfile << "," << results_anomalies->anomaly_id_of_explanation_pattern[i];
				myfile << endl;
			}
			myfile.close();

			// ANOMALY OUTLIERNESS
			if (append_to_file) {
				myfile.open(output_file + "_anomalies_outlierness", std::ios_base::app);
			}
			else {
				myfile.open(output_file + "_anomalies_outlierness");
				myfile << "outlierness";
				myfile << endl;
			}
			for (size_t i = 0; i < results_anomalies->anomaly_outlierness.size(); i++)
			{
				myfile << results_anomalies->anomaly_outlierness[i];
				myfile << endl;
			}
			myfile.close();

			// index snapshots from 1

			// OCCURRENCES
			if (append_to_file) {
				myfile.open(output_file + "_anomalies_occurrences", std::ios_base::app);
			}
			else {
				myfile.open(output_file + "_anomalies_occurrences");
				if (set_of_graphs)
				{
					myfile << "graphs" << endl;
				}
				else
				{
					myfile << "graph_snapshots" << endl;
				}
			}
			for (size_t i = 0; i < results_anomalies->anomaly_occurrences.size(); i++)
			{

				for (int j = 0; j < results_anomalies->anomaly_occurrences[i].size(); j++)
				{
					myfile << (results_anomalies->anomaly_occurrences[i][j] + 1);
					if (j < results_anomalies->anomaly_occurrences[i].size() - 1) {
						myfile << ",";
					}
				}

				myfile << endl;
			}
			myfile.close();
		}

		if (!append_to_file) {
			pu.outputEncodingToFile(output_file + "_encoding");
		}

		results->result_nodes.clear();
		results->result_edges.clear();
		results->support_absolute.clear();
		results->support.clear();
		results->confidence.clear();
		results->occurrences.clear();
		results->occurrences_antecedent.clear();

		if (search_for_anomalies)
		{
			results_anomalies->anomaly_result_nodes.clear();
			results_anomalies->anomaly_result_edges.clear();
			results_anomalies->anomaly_id_of_anomalous_pattern.clear();
			results_anomalies->anomaly_id_of_explanation_pattern.clear();
			results_anomalies->anomaly_outlierness.clear();
			results_anomalies->anomaly_occurrences.clear();
		}
	}

	void PartialUnion::setNewMeasures(bool new_measures) {
	 	this->new_measures = new_measures;
	}

	bool PartialUnion::getNewMeasures() {
	  	return new_measures;
	}

	PartialUnion::PartialUnion()
	{
		number_of_labels = 0;
	}

	PartialUnion::~PartialUnion()
	{
	}

	void PartialUnion::appendNode(std::array<int, 6> & node)
	{
		nodes.push_back(node);
	}
	void PartialUnion::appendEdge(std::array<int, 8> & edge)
	{
		edges.push_back(edge);
	}

	void PartialUnion::printAll() {
		using namespace std;

		int MAX_LIMIT = 50;

		int limit = 0;

		bool printed_dots = false;

		cout << "NODES (TIME, ID, LABEL, DELETED, CHANGETIME, CHANGETIMELINK):" << endl;
		for (array<int, 6> var : nodes)
		{
			if (limit > MAX_LIMIT && limit < nodes.size() - 20)
			{
				if (!printed_dots)
				{
					cout << "..." << endl;
					printed_dots = true;
				}
				limit++;
				continue;
			}
			for (int i = 0; i < 6; i++)
			{
				cout << var[i] << "\t";
			}
			cout << endl;

			limit++;

		}
		cout << endl;

		printed_dots = false;
		limit = 0;

		cout << "EDGES (TIME, SRC, DST, LABEL, DIRECTION, DELETED, CHANGETIME, ID):" << endl;
		for (array<int, 8> var : edges)
		{
			if (limit > MAX_LIMIT && limit < edges.size() - 20)
			{
				if (!printed_dots)
				{
					cout << "..." << endl;
					printed_dots = true;
				}
				limit++;
				continue;
			}
			for (int i = 0; i < 8; i++)
			{
				cout << var[i] << "\t";
			}
			cout << endl;

			limit++;
		}
		cout << endl;

		printEncoding();

	}
	void PartialUnion::printEncoding()
	{
		cout << "LABELING:" << endl;
		std::map<std::string, int>::iterator i;
		for (i = labelEncoding.begin(); i != labelEncoding.end(); i++)
		{
			std::cout << i->first << " " << i->second << " (OLD: " << antecedentLabelsOfLabels.find(i->second)->second << ")" << std::endl;
		}
	}

	void PartialUnion::printDimensions()
	{
		cout << "Number of nodes in the union graph: " << nodes.size() << endl;
		cout << "Number of edges in the union graph: " << edges.size() << endl;
	}

	int PartialUnion::getLabelEncoding(string label)
	{
		auto it = labelEncoding.find(label);
		if (it != labelEncoding.end()) {
			return it->second;
		}
		else {
			(number_of_labels)++;
			labelEncoding.insert(std::make_pair(label, number_of_labels));
			return number_of_labels;
		}
	}


	int PartialUnion::getAntecedentLabel(int label)
	{
		auto it = antecedentLabelsOfLabels.find(label);
		if (it != antecedentLabelsOfLabels.end()) {
			return it->second;
		}
		else
		{
			return -100;
		}
	}

	int PartialUnion::getDummyLabel(int label)
	{
		auto it = dummyLabelsOfLabels.find(label);
		if (it != dummyLabelsOfLabels.end()) {
			return it->second;
		}
		else
		{
			return -100;
		}
	}

	int PartialUnion::getAntecedentChangetime(int consequentChangetime)
	{
		if (consequentChangetime < 0)
		{
			return consequentChangetime + 1;
		}
		else
		{
			return -consequentChangetime + 1;
		}
	}

	std::string PartialUnion::getEncodingLabel(int value)
	{
		return encodingLabel[value];
	}

	void PartialUnion::appendToEncodingLabel(int encoding, string label)
	{
		encodingLabel.insert(std::make_pair(encoding, label));
	}

	void PartialUnion::tempAppendToEncoding(string label, int value)
	{
		labelEncoding.insert(std::make_pair(label, value));
	}

	void PartialUnion::appendToIsChange(int label, bool value)
	{
		labelIsChange.insert(std::make_pair(label, value));
	}


	void PartialUnion::appendToAntecedentLabelEncoding(int label, int oldlabel)
	{
		antecedentLabelsOfLabels.insert(std::make_pair(label, oldlabel));
	}

	void PartialUnion::appendToDummyLabelEncoding(int label, int oldlabel)
	{
		dummyLabelsOfLabels.insert(std::make_pair(label, oldlabel));
	}


	int PartialUnion::getNumberOfSnapshots()
	{
		return nodes.back()[0];
	}

	int PartialUnion::getNumberOfNodes()
	{
		return nodes.size();
	}

	void PartialUnion::setNumberOfDynamicGraphs(int number_of_graphs)
	{
		number_of_dynamic_graphs = number_of_graphs;
	}


	int PartialUnion::getNumberOfDynamicGraphs()
	{
		return number_of_dynamic_graphs;
	}

	void PartialUnion::appendToMappingSnapshotsToGraphs(int graph_number)
	{
		mappingSnapshotsToGraphs.push_back(graph_number);
	}

	int PartialUnion::queryMappingSnapshotsToGraphs(int snapshot)
	{
		return mappingSnapshotsToGraphs[snapshot];
	}

	void PartialUnion::performSimpleTimeAbstraction()
	{
		for (size_t i = 0; i < nodes.size(); i++)
		{
			if (nodes[i][PN_CHANGETIME] < 0)
			{
				nodes[i][PN_CHANGETIME] = -1;
			}
			else if (nodes[i][PN_CHANGETIME] > 0)
			{
				nodes[i][PN_CHANGETIME] = 1;
			}
			nodes[i][PN_CHANGETIMELINK] = (nodes[i][PN_CHANGETIMELINK] == nodes[i][PN_TIME]) ? 0 : -1;
		}

		for (size_t i = 0; i < edges.size(); i++)
		{
			if (edges[i][PE_CHANGETIME] < 0)
			{
				edges[i][PE_CHANGETIME] = -1;
			}
			else if (edges[i][PE_CHANGETIME] > 0)
			{
				edges[i][PE_CHANGETIME] = 1;
			}
		}

	}

	void PartialUnion::performSimpleNodeTimeAbstraction()
	{
		for (size_t i = 0; i < nodes.size(); i++)
		{
			if (nodes[i][PN_CHANGETIME] < 0)
			{
				nodes[i][PN_CHANGETIME] = -1;
			}
			else if (nodes[i][PN_CHANGETIME] > 0)
			{
				nodes[i][PN_CHANGETIME] = 1;
			}
			nodes[i][PN_CHANGETIMELINK] = (nodes[i][PN_CHANGETIMELINK] == nodes[i][PN_TIME]) ? 0 : -1;
		}

	}



	void PartialUnion::addLabeledNodeOccurrence(
		size_t node_index,
		int label,
		int changetime,
		std::vector<labeled_node> & labeled_nodes,
		bool set_of_graphs,
		int occurrence
	) {
		labeled_node ln;
		ln.label = label; // nodes[i][PN_LABEL];
		ln.changetime = changetime; // nodes[i][PN_CHANGETIME];
		auto it = std::find_if(labeled_nodes.begin(), labeled_nodes.end(), ln);
		if (it != std::end(labeled_nodes))
		{
		  	if (new_measures) {
			  	it->occurrences.insert(occurrence);
				it->multiple_occurrences[occurrence].insert(node_index);
		  	}
		  	else  {
			  	if (set_of_graphs) {
					it->occurrences.insert(queryMappingSnapshotsToGraphs(occurrence));
				} else {
					it->occurrences.insert(occurrence);
				}
		  	}

		}
		else
		{
		  	if (new_measures) {
			  	ln.occurrences.insert(occurrence);
				ln.multiple_occurrences[occurrence].insert(node_index);
			} else {
			  	if (set_of_graphs) {
					ln.occurrences.insert(queryMappingSnapshotsToGraphs(occurrence));
				} else {
					ln.occurrences.insert(occurrence);
				}
		  	}

			labeled_nodes.push_back(ln);
		}
	}

	size_t labeled_edge_with_occurrences::support(
		const std::vector<std::array<int, 8>>& edges,
		bool new_measures
	) const {
		if (new_measures) {
		  	if (!new_support.second) {
			  	size_t current_support = 0;
			  	for (auto const &snapshotId : occurrences) {
				  	OverlapGraph og;
				  	std::unordered_map<int, int> mapping;

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
						  	if (
						  		edges[*firstOccurrence][PE_SRC] == edges[*secondOccurrence][PE_SRC] ||
								edges[*firstOccurrence][PE_SRC] == edges[*secondOccurrence][PE_DST] ||
							  	edges[*firstOccurrence][PE_DST] == edges[*secondOccurrence][PE_SRC] ||
							  	edges[*firstOccurrence][PE_DST] == edges[*secondOccurrence][PE_DST]
							) {
								// there is a overlap between 2 occurrences
								og.addEdge(mapping.at(*firstOccurrence), mapping.at(*secondOccurrence));
							}
						}
					}

				  	current_support += og.computeSupport();
			  	}

			  	new_support = {current_support, true};
			  	return current_support;
		  	}

		  	return new_support.first;
		}

		return occurrences.size();
	}

	size_t labeled_node::support(bool new_measures) {
	  	if (new_measures) {
		  	size_t new_support = 0;
			for (auto snapshotId : occurrences) {
			  	new_support += multiple_occurrences.at(snapshotId).size();
			}

		  	return new_support;
		}
	  	return occurrences.size();
	}
	
	void PartialUnion::computeFrequentVerticesAndRemoveTheInfrequentOnes(int support_as_absolute, double min_confidence,
																		 bool compute_confidence, results_crate * results,
																		 results_crate_anomalies * results_anomalies,
																		 int max_absolute_support, bool set_of_graphs,
																		 bool search_for_anomalies, double min_anomaly_outlierness
																		)
	{
		// compute occurrences of the vertex patterns
		std::vector<labeled_node> labeled_nodes;
		for (size_t i = 0; i < nodes.size(); i++)
		{
			auto pos = labelIsChange.find(nodes[i][PN_LABEL]);
			if (pos != labelIsChange.end() && !pos->second) {
				// don't process "no-change" labels 
				continue;
			}
			addLabeledNodeOccurrence(
				i,
				nodes[i][PN_LABEL],
				nodes[i][PN_CHANGETIME],
				labeled_nodes,
				set_of_graphs,
				nodes[i][PN_TIME] - 1
			);
		}

		if (!compute_confidence)
		{
			// we are looking for frequent patterns, so delete the infrequent vertices
			// (but only if not computing confidence)
			nodes.erase(std::remove_if(std::begin(nodes), std::end(nodes), [support_as_absolute, &labeled_nodes, this](array<int, 6> node) {
				labeled_node ln;
				ln.label = node[PN_LABEL];
				ln.changetime = node[PN_CHANGETIME];
				auto it = std::find_if(labeled_nodes.begin(), labeled_nodes.end(), ln);
				if (it != std::end(labeled_nodes)) {
					if (it->support(new_measures) >= support_as_absolute) {
						return false;
					} else {
						return true;
					}
				} else {
					return false;
				}
			}), std::end(nodes));
		}

		// process the nodes and their occurrences:
		for (size_t i = 0; i < labeled_nodes.size(); i++)
		{
			// if the node is frequent,
			if (labeled_nodes[i].support(new_measures) >= support_as_absolute)
			{
				// if we compute confidence
				if (compute_confidence)
				{
					//int prev_label = antecedentLabelsOfLabels[labeled_nodes[i].label];
					int prev_label = getAntecedentLabel(labeled_nodes[i].label);
					//int prev_time = -(labeled_nodes[i].changetime) + 1;
					int prev_time = getAntecedentChangetime(labeled_nodes[i].changetime);
					int antecedent_abs_support = max_absolute_support;  // this default value is used for "addition" changes

					// for anomalies with regard to this pattern:
					std::vector<labeled_node> anomalous_vertices;

					std::set<int> antecedent_occurrences;
					std::unordered_map<int, std::set<int>> multiple_antecedent_occurrences;


					// if it is not an addition change
					if (prev_label != ANTECEDENT_LABEL_OF_ADDITION)
					{
						// find all occurrences with the same antecedent:

						for (size_t j = 0; j < nodes.size(); j++)
						{
							if (getAntecedentLabel(nodes[j][PN_LABEL]) == prev_label && getAntecedentChangetime(nodes[j][PN_CHANGETIME]) == prev_time)
							{
							  	if (new_measures) {
									multiple_antecedent_occurrences[nodes[j][PN_TIME] - 1].insert(j);
								  	antecedent_occurrences.insert(nodes[j][PN_TIME] - 1);
							  	} else {
								  	if (set_of_graphs) {
										antecedent_occurrences.insert(queryMappingSnapshotsToGraphs(nodes[j][PN_TIME] - 1));
									} else {
									  	antecedent_occurrences.insert(nodes[j][PN_TIME] - 1);
									}
							  	}

							}
						}
						if (new_measures) {
						  	antecedent_abs_support = 0;
						  	for (int snapshotId : antecedent_occurrences) {
							  	antecedent_abs_support += multiple_antecedent_occurrences[snapshotId].size();
						  	}
						} else {
						  	antecedent_abs_support = antecedent_occurrences.size();
						}
					}
					// if it is an addition change
					else
					{
						// put all snapshots to antecedent_occurrences:
						for (size_t k = 0; k < max_absolute_support; k++)
						{
							antecedent_occurrences.insert(k);
						}
					}
					// compute confidence of the pattern:
					double confidence = ((double)labeled_nodes[i].support(new_measures)) / ((double)antecedent_abs_support);

					
					// if the confidence is high enough:
					if (confidence >= min_confidence)
					{
						// save pattern to results:
						results->saved_instances++;
						std::array<int, 4> result_node = { results->saved_instances, 0, labeled_nodes[i].label, labeled_nodes[i].changetime };
						results->result_nodes.push_back(result_node);
						results->support_absolute.push_back(labeled_nodes[i].support(new_measures));
						results->support.push_back((double)labeled_nodes[i].occurrences.size() / (double)max_absolute_support);
						results->confidence.push_back(confidence);
						std::vector<int> used_occurrences(labeled_nodes[i].occurrences.begin(), labeled_nodes[i].occurrences.end());
						results->occurrences.push_back(used_occurrences);
						std::vector<int> used_antecedent_occurrences(antecedent_occurrences.begin(), antecedent_occurrences.end());
						results->occurrences_antecedent.push_back(used_antecedent_occurrences);


						// if we look also for anomalies
						if (search_for_anomalies)
						{
							if (labeled_nodes[i].changetime == 0)
							{
								// process anomalies for addition frequent pattern:
								labeled_node ln;
								ln.label = getDummyLabel(labeled_nodes[i].label);
								ln.changetime = 0;

								std::set<int> first_version_of_occurrences;

								if (set_of_graphs)
								{
									for (size_t k = 0; k < max_absolute_support; k++)
									{
										first_version_of_occurrences.insert(queryMappingSnapshotsToGraphs(k));
									}
								}
								else
								{
									for (size_t k = 0; k < max_absolute_support; k++)
									{
										first_version_of_occurrences.insert(k);
									}
								}

								std::set_difference(first_version_of_occurrences.begin(), first_version_of_occurrences.end(), labeled_nodes[i].occurrences.begin(), labeled_nodes[i].occurrences.end(),
									std::inserter(ln.occurrences, ln.occurrences.end()));

								// save only those that have at least one occurrence:
								if (ln.occurrences.size() > 0)
								{
									anomalous_vertices.push_back(ln);
								}
							}
							else
							{
								// find the "complement" patterns for non-addition-node frequent pattern:
								for (size_t j = 0; j < nodes.size(); j++)
								{
									// find the anomalous patterns:
									if (getAntecedentLabel(nodes[j][PN_LABEL]) == prev_label && getAntecedentChangetime(nodes[j][PN_CHANGETIME]) == prev_time &&
										(labeled_nodes[i].label != nodes[j][PN_LABEL] || labeled_nodes[i].changetime != nodes[j][PN_CHANGETIME]))
									{
										addLabeledNodeOccurrence(i, nodes[j][PN_LABEL], nodes[j][PN_CHANGETIME], anomalous_vertices, set_of_graphs, nodes[j][PN_TIME] - 1);
									}
								}
							}
							

							bool debug_print = false;

							// now save them:
							for (size_t j = 0; j < anomalous_vertices.size(); j++)
							{
								double anomaly_outlierness = 1 - ( ((double)anomalous_vertices[j].occurrences.size()) / ((double)antecedent_abs_support) );

								if (anomaly_outlierness >= min_anomaly_outlierness)
								{
									
									results_anomalies->anomaly_saved_instances++;
									std::array<int, 4> anomaly_result_node = { results_anomalies->anomaly_saved_instances, 0, anomalous_vertices[j].label, anomalous_vertices[j].changetime };
									results_anomalies->anomaly_result_nodes.push_back(anomaly_result_node);
									results_anomalies->anomaly_outlierness.push_back(anomaly_outlierness);
									results_anomalies->anomaly_id_of_explanation_pattern.push_back(results->saved_instances);
									results_anomalies->anomaly_id_of_anomalous_pattern.push_back(results_anomalies->anomaly_saved_instances);
									std::vector<int> anomaly_used_occurrences(anomalous_vertices[j].occurrences.begin(), anomalous_vertices[j].occurrences.end());
									results_anomalies->anomaly_occurrences.push_back(anomaly_used_occurrences);
								}
							}
						}
					}

				}
				// if we are not computing confidence (and certainly not computing anomalies)
				else {
					// JUST SAVE THEM
					results->saved_instances++;
					std::array<int, 4> result_node = { results->saved_instances, 0, labeled_nodes[i].label, labeled_nodes[i].changetime };
					results->result_nodes.push_back(result_node);
					results->support_absolute.push_back(labeled_nodes[i].support(new_measures));
					results->support.push_back((double)labeled_nodes[i].support(new_measures) / (double)max_absolute_support);
					std::vector<int> used_occurrences(labeled_nodes[i].occurrences.begin(), labeled_nodes[i].occurrences.end());
					results->occurrences.push_back(used_occurrences);
				}
			}
		}

	}


	// compares two labeled_edges (used for set structure)
	inline bool operator<(const labeled_edge_with_occurrences& lhs, const labeled_edge_with_occurrences& rhs)
	{
		// look at the first 7 elements only (dont take the 8th one - the ID of the edge)
		for (int i = 0; i < 7; i++) {
			if (lhs.elements[i] < rhs.elements[i])
			{
				return true;
			}
			else if (lhs.elements[i] > rhs.elements[i])
			{
				return false;
			}
		}
		return false;
	}
	// compares two labeled_edges (used for set structure)
	inline bool operator==(const labeled_edge_with_occurrences& lhs, const labeled_edge_with_occurrences& rhs)
	{
		// look at the first 7 elements only (dont take the 8th one - the ID of the edge)
		for (int i = 0; i < 7; i++)
		{
			if (lhs.elements[i] != rhs.elements[i])
			{
				return false;
			}
		}
		return true;
	}


	std::set<labeled_node_time> PartialUnion::compute_labeled_nodes()
	{
		std::set<labeled_node_time> labeled_nodes;
		for (size_t i = 0; i < nodes.size(); i++)
		{
			labeled_node_time lnt;
			lnt.time = nodes[i][PN_TIME];
			lnt.id = nodes[i][PN_ID];
			lnt.label = nodes[i][PN_LABEL];
			lnt.changetime = nodes[i][PN_CHANGETIME];
			labeled_nodes.insert(lnt);
		}
		return labeled_nodes;
	}

	void PartialUnion::eraseInvalidEdges(std::set<labeled_node_time> labeled_nodes)
	{
		edges.erase(std::remove_if(std::begin(edges), std::end(edges), [labeled_nodes](array<int, 8> edge) {
			labeled_node_time lnt_src;
			lnt_src.id = edge[PE_SRC];
			lnt_src.time = edge[PE_TIME];
			labeled_node_time lnt_dst;
			lnt_dst.id = edge[PE_DST];
			lnt_dst.time = edge[PE_TIME];

			return (labeled_nodes.find(lnt_src) == labeled_nodes.end() || labeled_nodes.find(lnt_dst) == labeled_nodes.end());
		}), std::end(edges));
	}


	void PartialUnion::compute_labeled_edges(std::vector<std::array<int, 8>> & newedges, std::set<labeled_edge_with_occurrences> & edges_set,
		std::set<labeled_node_time> & labeled_nodes, bool debugPrint = true)
	{
		labeled_node_time lnt_src;
		labeled_node_time lnt_dst;
		for (size_t i = 0; i < edges.size(); i++)
		{
			lnt_src.id = edges[i][PE_SRC];
			lnt_src.time = edges[i][PE_TIME];
			lnt_dst.id = edges[i][PE_DST];
			lnt_dst.time = edges[i][PE_TIME];
			auto search_src = labeled_nodes.find(lnt_src);
			auto search_dst = labeled_nodes.find(lnt_dst);

			array<int, 8> newedge = { search_src->label, search_src->changetime,
				edges[i][PE_DIRECTION], edges[i][PE_LABEL], edges[i][PE_CHANGETIME],
				search_dst->label, search_dst->changetime, edges[i][PE_ID] };


			newedges.push_back(newedge);

			labeled_edge_with_occurrences le;
			le.elements = smallerDirectionOfAdjacencyInfo(newedge);

			auto it = edges_set.find(le);

			if (it != std::end(edges_set))
			{

				(it->occurrences).insert(edges[i][PE_TIME] - 1); // - 1 because TIME is indexed from 1, but graphs from 0

			  	if (new_measures) {
				  	(it->multiple_occurrences[edges[i][PE_TIME] - 1]).insert(i);
				}
			}
			else
			{
				le.occurrences.insert(edges[i][PE_TIME] - 1); // - 1 because TIME is indexed from 1, but graphs from 0

			  	if (new_measures) {
					le.multiple_occurrences[edges[i][PE_TIME] - 1].insert(i);
				}

			  	edges_set.insert(le);
			}
		}
	}

	void PartialUnion::removeInfrequentEdges(std::vector<std::array<int, 8>> & newedges, std::set<labeled_edge_with_occurrences> & edges_set,
		int support_as_absolute, bool set_of_graphs, bool debugPrint = false)
	{
		int count = 0;
		edges.erase(std::remove_if(std::begin(edges), std::end(edges), [&edges_set, support_as_absolute, newedges, &count, set_of_graphs, this](array<int, 8> edge) mutable {
			std::array<int, 8> ne = newedges[count];
			labeled_edge_with_occurrences le;
			le.elements = smallerDirectionOfAdjacencyInfo(ne);
			++count;

		  	std::set<labeled_edge_with_occurrences>::iterator it = edges_set.find(le);

			if (set_of_graphs)
			{
				std::set<int> mapped_occurrences;
				std::set<int>::iterator it2;
				for (it2 = it->occurrences.begin(); it2 != it->occurrences.end(); ++it2)
				{
					mapped_occurrences.insert(queryMappingSnapshotsToGraphs(*it2));
				}

				return (((!new_measures && mapped_occurrences.size() < support_as_absolute) || (new_measures && it->support(edges, true) < support_as_absolute))
					&& (it->elements[ADJ_INFO_CHANGETIME] >= 0 || it->elements[ADJ_INFO_SRC_CHANGETIME] >= 0 || it->elements[ADJ_INFO_DST_CHANGETIME] >= 0));
			}
			else
			{
				// only change edges can be infrequent (i.e. any of their changetimes >= 0)
				return (it->support(edges, new_measures) < support_as_absolute
					&& (it->elements[ADJ_INFO_CHANGETIME] >= 0 || it->elements[ADJ_INFO_SRC_CHANGETIME] >= 0 || it->elements[ADJ_INFO_DST_CHANGETIME] >= 0));
			}

		}), std::end(edges));


		newedges.erase(std::remove_if(std::begin(newedges), std::end(newedges), [edges_set, support_as_absolute, set_of_graphs, this](array<int, 8> edge) {
			labeled_edge_with_occurrences le;
			le.elements = smallerDirectionOfAdjacencyInfo(edge);

			auto it = edges_set.find(le);

			if (set_of_graphs)
			{
				std::set<int> mapped_occurrences;
				std::set<int>::iterator it2;
				for (it2 = it->occurrences.begin(); it2 != it->occurrences.end(); ++it2)
				{
					mapped_occurrences.insert(queryMappingSnapshotsToGraphs(*it2));
				}
				return (((!new_measures && mapped_occurrences.size() < support_as_absolute) || (new_measures && it->support(edges, true) < support_as_absolute))
					&& (it->elements[ADJ_INFO_CHANGETIME] >= 0 || it->elements[ADJ_INFO_SRC_CHANGETIME] >= 0 || it->elements[ADJ_INFO_DST_CHANGETIME] >= 0));
			}
			else
			{
				return (it->support(edges, new_measures) < support_as_absolute
					&& (it->elements[ADJ_INFO_CHANGETIME] >= 0 || it->elements[ADJ_INFO_SRC_CHANGETIME] >= 0 || it->elements[ADJ_INFO_DST_CHANGETIME] >= 0));
			}

		}), std::end(newedges));
	}


	std::vector<AdjacencyListCrate> PartialUnion::createAdjacencyLists(std::vector<std::array<int, 8>> & newedges, int snapshots)
	{
		// RENAME IDS (so IDS are from 0 to N)
		int current_snapshot = 1;  // because first time stamp == 1
		std::map<int, int> new_node_ids;
		int id_counter = 0;

		vector<int> numbers_of_nodes; // how many nodes there are in each snapshot


		size_t edges_processed_index = 0;
		for (size_t i = 0; i <= nodes.size(); i++)
		{

			if (i == nodes.size() || nodes[i][PN_TIME] > current_snapshot)
			{
				// doing next snapshot
				numbers_of_nodes.push_back(id_counter);

				// rename edges
				for (; edges_processed_index < edges.size() && edges[edges_processed_index][PE_TIME] <= current_snapshot; edges_processed_index++)
				{
					edges[edges_processed_index][PE_SRC] = new_node_ids.find(edges[edges_processed_index][PE_SRC])->second;
					edges[edges_processed_index][PE_DST] = new_node_ids.find(edges[edges_processed_index][PE_DST])->second;
				}

				// after all elements are processed, end
				if (i == nodes.size()) break;
				// reset counters:

				current_snapshot++;

				id_counter = 0;
				new_node_ids.clear();

			}

			new_node_ids.insert(std::make_pair(nodes[i][PN_ID], id_counter));

			// rename node ids:
			nodes[i][PN_ID] = id_counter;

			id_counter++;
		}

		// BUILD ADJACENCY LISTS:
		std::vector<AdjacencyListCrate> adjacency_lists;
		current_snapshot = 1;
		size_t e_index = 0;
		size_t n_index = 0;
		for (int current_snapshot = 1; current_snapshot <= snapshots; current_snapshot++)
		{
			// for each snapshot
			// initialize vectors of appropriate sizes:
			std::vector<std::vector<int>> adjacencyList(numbers_of_nodes[current_snapshot - 1], std::vector<int>(0));
			std::vector<std::vector<std::array<int, ADJ_INFO___SIZE>>> adjacencyMoreInfo(
				numbers_of_nodes[current_snapshot - 1],
				std::vector<std::array<int, ADJ_INFO___SIZE>>(0)
			);
			std::vector<std::array<int, ADJ_NODES___SIZE>> adjacencyListNodes; // NEW

			while (n_index < nodes.size() && nodes[n_index][PN_TIME] <= current_snapshot)
			{
				// fill all nodes in the current snapshot:
				std::array<int, ADJ_NODES___SIZE> node = { nodes[n_index][PN_ID], nodes[n_index][PN_CHANGETIME], nodes[n_index][PN_LABEL] };
				adjacencyListNodes.push_back(node);
				n_index++;
			}
			while (e_index < edges.size() && edges[e_index][PE_TIME] <= current_snapshot)
			{
				// fill all edges of the current snapshot:
				adjacencyList[edges[e_index][PE_SRC]].push_back(edges[e_index][PE_DST]);
				adjacencyList[edges[e_index][PE_DST]].push_back(edges[e_index][PE_SRC]);
				adjacencyMoreInfo[edges[e_index][PE_SRC]].push_back(newedges[e_index]);
				adjacencyMoreInfo[edges[e_index][PE_DST]].push_back(flipAdjacencyInfo(newedges[e_index]));
				e_index++;
			}
			// create new struct element
			AdjacencyListCrate alc;
			alc.adjacencyList = adjacencyList;
			alc.adjacencyEdgeInfo = adjacencyMoreInfo;
			alc.nodes = adjacencyListNodes;
			adjacency_lists.push_back(alc);
		}


		return adjacency_lists;
	}

	bool PartialUnion::matches_the_adj_info_to_antecedent_pattern_edge(const std::array<int, ADJ_INFO___SIZE> &adj_info, const std::array<int, PAT___SIZE> &edge, bool ignore_edge_id)
	{
		return getAntecedentLabel(adj_info[ADJ_INFO_SRC_LABEL]) == getAntecedentLabel(edge[PAT_SRC_LABEL]) &&
			getAntecedentChangetime(adj_info[ADJ_INFO_SRC_CHANGETIME]) == getAntecedentChangetime(edge[PAT_SRC_CHANGETIME]) &&
			adj_info[ADJ_INFO_DIRECTION] == edge[PAT_DIRECTION] &&
			getAntecedentLabel(adj_info[ADJ_INFO_LABEL]) == getAntecedentLabel(edge[PAT_LABEL]) &&
			getAntecedentChangetime(adj_info[ADJ_INFO_CHANGETIME]) == getAntecedentChangetime(edge[PAT_CHANGETIME]) &&
			getAntecedentLabel(adj_info[ADJ_INFO_DST_LABEL]) == getAntecedentLabel(edge[PAT_DST_LABEL]) &&
			getAntecedentChangetime(adj_info[ADJ_INFO_DST_CHANGETIME]) == getAntecedentChangetime(edge[PAT_DST_CHANGETIME]);
	}

	void PartialUnion::outputEncodingToFile(string filename)
	{
		ofstream myfile;

		myfile.open(filename);

		myfile << "label,label_int,is_change,label_int_prev" << endl;
		for (std::map<std::string, int>::iterator it = labelEncoding.begin(); it != labelEncoding.end(); ++it)
		{
			myfile << "\"" << it->first << "\"," << it->second << "," << labelIsChange[it->second] << "," << antecedentLabelsOfLabels[it->second] << endl;
		}

		myfile.close();
	}
	void PartialUnion::debug_printEdges()
	{
		for (size_t i = 0; i < edges.size(); i++)
		{
			for (size_t j = 0; j < edges[i].size(); j++)
			{
				std::cout << edges[i][j] << "\t";
			}
			std::cout << std::endl;
		}
	}
	void PartialUnion::debug_printNodes()
	{
		for (size_t i = 0; i < nodes.size(); i++)
		{
			for (size_t j = 0; j < nodes[i].size(); j++)
			{
				std::cout << nodes[i][j] << "\t";
			}
			std::cout << std::endl;
		}
	}

	std::vector<std::array<int, 8>> PartialUnion::getEdges() const {
	  	return edges;
	}

}
