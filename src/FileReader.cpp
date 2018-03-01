
#include "FileReader.h"
#include "HelperFunctions.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <set>
#include <vector>

#include <locale>
// #include <codecvt>
#include <string>
#include <cstdlib>
#include <algorithm>


namespace dgrminer {

	vector<string> FileReader::split(string str, char delimiter) {
		vector<string> internal;
		stringstream ss(str); // Turn the string into a stream.
		string tok;

		while (getline(ss, tok, delimiter)) {
			internal.push_back(tok);
		}

		return internal;
	}


	PartialUnion FileReader::read_dynamic_graph_from_file(std::string filename, int window_size, bool search_for_anomalies) {

		bool check_format_correctness = true;
		bool dont_keep_deleted_elements = true;

		std::ifstream infile(filename);

		// nodes and edges of the current snapshot:
		std::vector<std::array<int, 7>> currentNodes; // indexed by FR_PN_ constants
		std::vector<std::array<int, 9>> currentEdges; // indexed by FR_PE_ constants

		std::vector<int> currentNodesChangetimeRelative;
		std::vector<int> currentEdgesChangetimeRelative;
		
		std::set<int> currently_deleted_nodes; // node IDS which were deleted in the current snapshot - used for checking that the appropriate edges were also deleted

		// result will be here:
		PartialUnion pu;

		bool doing_init_part;
		int global_snapshot_number = 0;

		int id1;
		int id2;
		int id3;
		int label;
		int label_old;
		int changelabel;
		int direction;

		// used for anomalies:
		int label_non_addition;

		// this variables tells us which dynamic graph we are doing:
		int graph_number = -1; // so that our first graph has number 0;

		bool done_init_part = false;

		int line_number = 0; // for check_format_correctness


		for (std::string line; getline(infile, line);)
		{
			line_number++;

			if (check_format_correctness && line_number == 1 && line != "init")
			{
				fail_with_message("File does not start with 'init' line!", line_number);
			}

			if (line == "init") {
				
				currentNodes.clear();
				currentEdges.clear();
				currentNodesChangetimeRelative.clear();
				currentEdgesChangetimeRelative.clear();

				graph_number++;

				doing_init_part = true;
			}
			else if (line == "changes") {
				if (check_format_correctness && !doing_init_part)
				{
					fail_with_message("'init' section must come before 'changes'!", line_number);
				}

				doing_init_part = false;
			}

			

			if (doing_init_part)
			{

				// DO INITIAL PART:

				// vertex
				if (line[0] == 'v')
				{
					done_init_part = true;

					vector<string> x = split(line, ' ');

					if (check_format_correctness && x.size() != 3) fail_with_message("Line with vertex initialization must contain 3 elements: 'v ID_V LAB'", line_number);

					label = pu.getLabelEncoding(x[2]);
					changelabel = pu.getLabelEncoding("+" + x[2]);
					pu.appendToEncodingLabel(label, x[2]);
					pu.appendToEncodingLabel(changelabel, "+" + x[2]);
					
					pu.appendToIsChange(label, false);
					pu.appendToIsChange(changelabel, true);
					pu.appendToAntecedentLabelEncoding(label, label);
					pu.appendToAntecedentLabelEncoding(changelabel, -1);

					if (search_for_anomalies)
					{
						label_non_addition = pu.getLabelEncoding("!" + x[2]);
						pu.appendToEncodingLabel(label_non_addition, "!" + x[2]);
						pu.appendToIsChange(label_non_addition, false);
						pu.appendToAntecedentLabelEncoding(label_non_addition, -1);

						pu.appendToDummyLabelEncoding(label, -1);
						pu.appendToDummyLabelEncoding(changelabel, label_non_addition);
						pu.appendToDummyLabelEncoding(label_non_addition, -1);
					}

					try
					{
						id1 = std::stoi(x[1]);
					}
					catch (std::exception& e)
					{
						fail_with_message("Line with vertex initialization does not use integer ID!", line_number);
					}
					

					array<int, 7> values = { 1, id1, label, 0, global_snapshot_number, global_snapshot_number, changelabel };
					currentNodes.push_back(values);
					currentNodesChangetimeRelative.push_back(0);

				}
				// undirected edge or directed edge
				else if (line[0] == 'u' || line[0] == 'd')
				{
					done_init_part = true;

					vector<string> x = split(line, ' ');

					if (check_format_correctness && x.size() != 5) fail_with_message("Line with edge initialization must contain 5 elements: 'u ID_E ID_F ID_T LAB' or 'v ID_E ID_F ID_T LAB'", line_number);

					direction = getDirectionInt(&x[0]);

					label = pu.getLabelEncoding(x[4]);
					changelabel = pu.getLabelEncoding("+" + x[4]);
					pu.appendToEncodingLabel(label, x[4]);
					pu.appendToEncodingLabel(changelabel, "+" + x[4]);
					
					pu.appendToIsChange(label, false);
					pu.appendToIsChange(changelabel, true);
					pu.appendToAntecedentLabelEncoding(label, label);
					pu.appendToAntecedentLabelEncoding(changelabel, -1);

					if (search_for_anomalies)
					{
						label_non_addition = pu.getLabelEncoding("!" + x[4]);
						pu.appendToEncodingLabel(label_non_addition, "!" + x[4]);
						pu.appendToIsChange(label_non_addition, false);
						pu.appendToAntecedentLabelEncoding(label_non_addition, -1);

						pu.appendToDummyLabelEncoding(label, -1);
						pu.appendToDummyLabelEncoding(changelabel, label_non_addition);
						pu.appendToDummyLabelEncoding(label_non_addition, -1);
					}

					try
					{
						id1 = std::stoi(x[1]);
						id2 = std::stoi(x[2]);
						id3 = std::stoi(x[3]);
					}
					catch (std::exception& e)
					{
						fail_with_message("Line with edge initialization does not use integer IDs!", line_number);
					}

					// check whether we have nodes with both ids
					if (check_format_correctness)
					{
						bool id2_is_there = false;
						bool id3_is_there = false;
						for (size_t i = 0; i < currentNodes.size(); i++)
						{
							if (currentNodes[i][FR_PN_ID] == id2)
							{
								id2_is_there = true;
							}
							else if (currentNodes[i][FR_PN_ID] == id3)
							{
								id3_is_there = true;
							}
						}
						if (!id2_is_there) fail_with_message("There is no node with id=" + std::to_string(id2) + " for the added edge!", line_number);
						if (!id3_is_there) fail_with_message("There is no node with id=" + std::to_string(id3) + " for the added edge!", line_number);
					}

					array<int, 9> values = { 1, id2, id3,
						label, direction, 0, global_snapshot_number, changelabel, id1 };
					currentEdges.push_back(values);
					currentEdgesChangetimeRelative.push_back(0);

				}

				continue; // skip the rest for now
			} // if (doing_init_part)


			else if (line[0] == 't' || line == "end") {
				// at the end of each snapshot, process currentNodes and currentEdges

				// if there were some vertex deletions in the currently processed snapshot, check also all edges:
				if (check_format_correctness && currently_deleted_nodes.size() > 0)
				{
					for (size_t i = 0; i < currentEdges.size(); i++)
					{
						if (currentEdges[i][FR_PE_DELETED] != 1 && currently_deleted_nodes.find(currentEdges[i][FR_PE_SRC]) != currently_deleted_nodes.end())
						{
							fail_with_message("There is an edge with vertex ids=" + std::to_string(currentEdges[i][FR_PE_SRC]) + "," + std::to_string(currentEdges[i][FR_PE_DST])
								+ " and label=" + pu.getEncodingLabel(currentEdges[i][FR_PE_LABEL]) + " " + std::to_string(currentEdges[i][FR_PE_DELETED])
								+ " which was not deleted, even though vertex with id=" + std::to_string(currentEdges[i][FR_PE_SRC])
									+ " was deleted! Problem is in the last snapshot preceding this line.", line_number);
						}
						if (currentEdges[i][FR_PE_DELETED] != 1 && currently_deleted_nodes.find(currentEdges[i][FR_PE_DST]) != currently_deleted_nodes.end())
						{
							fail_with_message("There is an edge with vertex ids=" + std::to_string(currentEdges[i][FR_PE_SRC]) + "," + std::to_string(currentEdges[i][FR_PE_DST])
								+ " and label=" + pu.getEncodingLabel(currentEdges[i][FR_PE_LABEL]) + " " + std::to_string(currentEdges[i][FR_PE_DELETED])
								+ " which was not deleted, even though vertex with id=" + std::to_string(currentEdges[i][FR_PE_DST])
								+ " was deleted! Problem is in the last snapshot preceding this line.", line_number);
						}
					}
				}

				std::set<int> old_vertex_ids; // used when window_size > 0, keeps vertex ids which should be removed (these vertices are too old)

				// save relevant currentNodes to PartialUnion structure
				for (size_t i = 0; i < currentNodes.size(); i++)
				{
					// if the node is older than window
					bool should_be_removed = window_size > 0 && ((global_snapshot_number - currentNodes[i][FR_PN_CHANGETIMELINK]) >= window_size);
					// or it was deleted some time ago (we don't want to keep the old deleted elements):
					should_be_removed = should_be_removed || (dont_keep_deleted_elements && currentNodes[i][FR_PN_DELETED] == 1 && currentNodesChangetimeRelative[i] < 0);

					if (should_be_removed)
					{
						// such a vertex will be deleted (save it here, so we can delete also adjacent edges)
						old_vertex_ids.insert(currentNodes[i][FR_PN_ID]);
					}
					else
					{
						if (!done_init_part)
						{
							array<int, 6> values = { global_snapshot_number, currentNodes[i][FR_PN_ID],
								(currentNodesChangetimeRelative[i] < 0) ? currentNodes[i][FR_PN_LABEL] : currentNodes[i][FR_PN_CHANGELABEL],
								currentNodes[i][FR_PN_DELETED],
								currentNodesChangetimeRelative[i],
								currentNodes[i][FR_PN_CHANGETIMELINK]
							};
							pu.appendNode(values);
						}
					}
				}
				// save relevant currentEdges to PartialUnion structure
				for (size_t i = 0; i < currentEdges.size(); i++)
				{
					// if the edge is older than window
					bool should_be_removed = window_size > 0 && (((global_snapshot_number - currentEdges[i][FR_PE_CHANGETIME]) >= window_size)
						|| (old_vertex_ids.find(currentEdges[i][FR_PE_SRC]) != old_vertex_ids.end())
						|| (old_vertex_ids.find(currentEdges[i][FR_PE_DST]) != old_vertex_ids.end()));
					// or it was deleted some time ago (we don't want to keep the old deleted elements):
					should_be_removed = should_be_removed || (dont_keep_deleted_elements && currentEdges[i][FR_PE_DELETED] == 1 && currentEdgesChangetimeRelative[i] < 0);

					if (should_be_removed)
					{
						// PASS
					}
					else
					{
						if (!done_init_part)
						{
							currentEdges[i][FR_PE_TIME] = global_snapshot_number;
							array<int, 8> values = { global_snapshot_number,
								currentEdges[i][FR_PE_SRC],
								currentEdges[i][FR_PE_DST],
								(currentEdgesChangetimeRelative[i] < 0) ? currentEdges[i][FR_PE_LABEL] : currentEdges[i][FR_PE_CHANGELABEL],
								currentEdges[i][FR_PE_DIRECTION],
								currentEdges[i][FR_PE_DELETED],
								currentEdgesChangetimeRelative[i],
								currentEdges[i][FR_PE_ID]
							};
							pu.appendEdge(values);
						}
					}
				}

				done_init_part = false;

				// if we moved to next snapshot and not to the end of the dynamic graph
				if (line[0] == 't')
				{
					global_snapshot_number++;
					/*if (set_of_graphs)
					{
						pu.appendToMappingSnapshotsToGraphs(graph_number);
					}*/
					pu.appendToMappingSnapshotsToGraphs(graph_number);
				}
				

				for (size_t i = 0; i < currentNodes.size(); i++)
				{
					currentNodesChangetimeRelative[i] = currentNodes[i][FR_PN_CHANGETIME] - global_snapshot_number;
				}
				for (size_t i = 0; i < currentEdges.size(); i++)
				{
					currentEdgesChangetimeRelative[i] = currentEdges[i][FR_PE_CHANGETIME] - global_snapshot_number;
				}

				currently_deleted_nodes.clear();

			} // if (line[0] == 't' || line == "end")



			// node addition
			else if (line[0] == 'a' && line[1] == 'n') {

				vector<string> x = split(line, ' ');

				if (check_format_correctness && x.size() != 3) fail_with_message("Line with vertex addition must contain 3 elements: 'an ID_V LAB'", line_number);

				label = pu.getLabelEncoding(x[2]);
				changelabel = pu.getLabelEncoding("+"+x[2]);
				pu.appendToEncodingLabel(label, x[2]);
				pu.appendToEncodingLabel(changelabel, "+" + x[2]);
				
				pu.appendToIsChange(label, false);
				pu.appendToIsChange(changelabel, true);
				pu.appendToAntecedentLabelEncoding(label, label);
				pu.appendToAntecedentLabelEncoding(changelabel, -1);

				if (search_for_anomalies)
				{
					label_non_addition = pu.getLabelEncoding("!" + x[2]);
					pu.appendToEncodingLabel(label_non_addition, "!" + x[2]);
					pu.appendToIsChange(label_non_addition, false);
					pu.appendToAntecedentLabelEncoding(label_non_addition, -1);

					pu.appendToDummyLabelEncoding(label, -1);
					pu.appendToDummyLabelEncoding(changelabel, label_non_addition);
					pu.appendToDummyLabelEncoding(label_non_addition, -1);
				}

				try
				{
					id1 = std::stoi(x[1]);
				}
				catch (std::exception& e)
				{
					fail_with_message("Line with vertex addition does not use integer ID!", line_number);
				}

				array<int, 7> values = { global_snapshot_number, id1, label, 0, global_snapshot_number, global_snapshot_number, changelabel };
				currentNodes.push_back(values);
				currentNodesChangetimeRelative.push_back(0);

			}
			// edge addition
			else if (line[0] == 'a' && line[1] == 'e') {

				vector<string> x = split(line, ' ');

				if (check_format_correctness && x.size() != 6) fail_with_message("Line with edge addition must contain 6 elements: 'ae u ID_E ID_F ID_T LAB' or 'ae d ID_E ID_F ID_T LAB'", line_number);

				direction = getDirectionInt(&x[1]);

				label = pu.getLabelEncoding(x[5]);
				changelabel = pu.getLabelEncoding("+" + x[5]);
				pu.appendToEncodingLabel(label, x[5]);
				pu.appendToEncodingLabel(changelabel, "+" + x[5]);

				pu.appendToIsChange(label, false);
				pu.appendToIsChange(changelabel, true);
				pu.appendToAntecedentLabelEncoding(label, label);
				pu.appendToAntecedentLabelEncoding(changelabel, -1);

				if (search_for_anomalies)
				{
					label_non_addition = pu.getLabelEncoding("!" + x[5]);
					pu.appendToEncodingLabel(label_non_addition, "!" + x[5]);
					pu.appendToIsChange(label_non_addition, false);
					pu.appendToAntecedentLabelEncoding(label_non_addition, -1);

					pu.appendToDummyLabelEncoding(label, -1);
					pu.appendToDummyLabelEncoding(changelabel, label_non_addition);
					pu.appendToDummyLabelEncoding(label_non_addition, -1);
				}

				try
				{
					id1 = std::stoi(x[2]);
					id2 = std::stoi(x[3]);
					id3 = std::stoi(x[4]);
				}
				catch (std::exception& e)
				{
					fail_with_message("Line with edge addition does not use integer ID!", line_number);
				}

				bool updated = false;
				for (size_t i = 0; i < currentEdges.size(); i++)
				{
					/*if (currentEdges[i][FR_PE_FROM] == id2 && currentEdges[i][FR_PE_TO] == id3
						&& currentEdges[i][FR_PE_DIRECTION] == direction
						&& currentEdges[i][FR_PE_LABEL] == label)*/
					// if there already is an edge with the given id, refresh it:
					if (currentEdges[i][FR_PE_ID] == id1)
					{
						currentEdges[i][FR_PE_DELETED] = 0;
						currentEdges[i][FR_PE_CHANGETIME] = global_snapshot_number;
						currentEdges[i][FR_PE_CHANGELABEL] = changelabel;
						currentEdgesChangetimeRelative[i] = 0;
						
						updated = true;
						break;
					}
				}

				if (!updated)
				{
					array<int, 9> values = { global_snapshot_number, id2, id3,
						label, direction, 0, global_snapshot_number, changelabel, id1 };
					currentEdges.push_back(values);
					currentEdgesChangetimeRelative.push_back(0);
				}
				
				// update changetimelink of nodes

				bool id2_is_there = false;
				bool id3_is_there = false;
				for (size_t j = 0; j < currentNodes.size(); j++)
				{
					if (currentNodes[j][FR_PN_ID] == id2)
					{
						id2_is_there = true;
						currentNodes[j][FR_PN_CHANGETIMELINK] = global_snapshot_number;
					}
					else if (currentNodes[j][FR_PN_ID] == id3)
					{
						id3_is_there = true;
						currentNodes[j][FR_PN_CHANGETIMELINK] = global_snapshot_number;
					}
				}
				if (!id2_is_there) fail_with_message("There is no node with id=" + std::to_string(id2) + " for the added edge!", line_number);
				if (!id3_is_there) fail_with_message("There is no node with id=" + std::to_string(id3) + " for the added edge!", line_number);

			}
			else if (line[0] == 'c' && line[1] == 'n') {
				//std::cout << "node change" << std::endl;

				vector<string> x = split(line, ' ');

				if (check_format_correctness && x.size() != 3) fail_with_message("Line with vertex label change must contain 3 elements: 'cn ID_V LAB'", line_number);

				label = pu.getLabelEncoding(x[2]);
				pu.appendToEncodingLabel(label, x[2]);
				pu.appendToIsChange(label, false);
				pu.appendToAntecedentLabelEncoding(label, label);

				if (search_for_anomalies)
				{
					pu.appendToDummyLabelEncoding(label, -1);
				}

				try
				{
					id1 = std::stoi(x[1]);
				}
				catch (std::exception& e)
				{
					fail_with_message("Line with vertex label change does not use integer ID!", line_number);
				}
				
				bool found_node = false;
				for (size_t i = 0; i < currentNodes.size(); i++)
				{
					if (currentNodes[i][FR_PN_ID] == id1) {
						found_node = true;
						changelabel = pu.getLabelEncoding(pu.getEncodingLabel(currentNodes[i][FR_PN_LABEL]) + "=>" + x[2]);
						pu.appendToEncodingLabel(changelabel, pu.getEncodingLabel(currentNodes[i][FR_PN_LABEL]) + "=>" + x[2]);


						//cout << "LABEL: " << label << " " << x[2] << " CHANGELABEL: " << changelabel << " " << (pu.getEncodingLabel(currentNodes[i][FR_PN_LABEL]) + "=>" + x[2]) << endl;

						pu.appendToIsChange(changelabel, true);
						pu.appendToAntecedentLabelEncoding(changelabel, currentNodes[i][FR_PN_LABEL]);

						if (search_for_anomalies)
						{
							pu.appendToDummyLabelEncoding(changelabel, -1);
						}

						currentNodes[i][FR_PN_LABEL] = label;
						currentNodes[i][FR_PN_CHANGELABEL] = changelabel;
						currentNodesChangetimeRelative[i] = global_snapshot_number - currentNodes[i][FR_PN_CHANGETIME];
						currentNodes[i][FR_PN_CHANGETIME] = global_snapshot_number;
						currentNodes[i][FR_PN_CHANGETIMELINK] = global_snapshot_number;

						break;
					}
				}
				if (check_format_correctness && !found_node)
				{
					fail_with_message("There is no node with id=" + std::to_string(id1) + " for label change!", line_number);
				}
				
			}
			else if (line[0] == 'c' && line[1] == 'e') {
				//std::cout << "edge change" << std::endl;

				vector<string> x = split(line, ' ');

				if (check_format_correctness && x.size() != 3) fail_with_message("Line with edge label change must contain 3 elements: 'ce ID_E LAB'", line_number);

				label = pu.getLabelEncoding(x[2]);
				pu.appendToEncodingLabel(label, x[2]);
				pu.appendToIsChange(label, false);
				pu.appendToAntecedentLabelEncoding(label, label);

				if (search_for_anomalies)
				{
					pu.appendToDummyLabelEncoding(label, -1);
				}

				try
				{
					id1 = std::stoi(x[1]);
				}
				catch (std::exception& e)
				{
					fail_with_message("Line with edge label change does not use integer ID!", line_number);
				}

				bool found_edge = false;
				for (size_t i = 0; i < currentEdges.size(); i++)
				{
					if (currentEdges[i][FR_PE_ID] == id1)
					//if (currentEdges[i][FR_PE_FROM] == id1 && currentEdges[i][FR_PE_TO] == id2
					//	&& currentEdges[i][FR_PE_LABEL] == label_old)
					{
						found_edge = true;
						changelabel = pu.getLabelEncoding(pu.getEncodingLabel(currentEdges[i][FR_PE_LABEL]) + "=>" + x[2]);
						pu.appendToEncodingLabel(changelabel, pu.getEncodingLabel(currentEdges[i][FR_PE_LABEL]) + "=>" + x[2]);
						
						pu.appendToIsChange(changelabel, true);
						pu.appendToAntecedentLabelEncoding(changelabel, currentEdges[i][FR_PE_LABEL]);

						if (search_for_anomalies)
						{
							pu.appendToDummyLabelEncoding(changelabel, -1);
						}

						currentEdges[i][FR_PE_LABEL] = label;
						currentEdges[i][FR_PE_CHANGELABEL] = changelabel;
						currentEdgesChangetimeRelative[i] = global_snapshot_number - currentEdges[i][FR_PE_CHANGETIME];
						currentEdges[i][FR_PE_CHANGETIME] = global_snapshot_number;

						// update changetimelink of the corresponding vertices:
						id2 = currentEdges[i][FR_PE_SRC];
						id3 = currentEdges[i][FR_PE_DST];
						for (size_t j = 0; j < currentNodes.size(); j++)
						{
							if ((currentNodes[j][FR_PN_ID] == id2) || (currentNodes[j][FR_PN_ID] == id3))
							{
								currentNodes[j][FR_PN_CHANGETIMELINK] = global_snapshot_number;
							}
						}

						break;
					}
				}

				if (check_format_correctness && !found_edge)
				{
					fail_with_message("There is no edge with id=" + std::to_string(id1) + " for label change!", line_number);
				}

			}
			else if (line[0] == 'd' && line[1] == 'n') {
				//std::cout << "node deletion" << std::endl;
				
				vector<string> x = split(line, ' ');

				if (check_format_correctness && x.size() != 2) fail_with_message("Line with vertex deletion must contain 2 elements: 'dn ID_V'", line_number);

				try
				{
					id1 = std::stoi(x[1]);
				}
				catch (std::exception& e)
				{
					fail_with_message("Line with vertex deletion does not use integer ID!", line_number);
				}
				currently_deleted_nodes.insert(id1);

				bool found_node = false;
				for (size_t i = 0; i < currentNodes.size(); i++)
				{
					if (currentNodes[i][FR_PN_ID] == id1) {
						found_node = true;
						changelabel = pu.getLabelEncoding("-" + pu.getEncodingLabel(currentNodes[i][FR_PN_LABEL]));
						pu.appendToEncodingLabel(changelabel, "-" + pu.getEncodingLabel(currentNodes[i][FR_PN_LABEL]));

						//cout << "LABEL: -, CHANGELABEL: " << changelabel << " " << ("-" + pu.getEncodingLabel(currentNodes[i][FR_PN_LABEL])) << endl;

						pu.appendToIsChange(changelabel, true);
						pu.appendToAntecedentLabelEncoding(changelabel, currentNodes[i][FR_PN_LABEL]);

						if (search_for_anomalies)
						{
							pu.appendToDummyLabelEncoding(changelabel, -1);
						}

						currentNodes[i][FR_PN_CHANGELABEL] = changelabel;
						currentNodesChangetimeRelative[i] = global_snapshot_number - currentNodes[i][FR_PN_CHANGETIME];
						currentNodes[i][FR_PN_CHANGETIME] = global_snapshot_number;
						currentNodes[i][FR_PN_CHANGETIMELINK] = global_snapshot_number;
						currentNodes[i][FR_PN_DELETED] = 1;
						break;
					}
				}

				if (check_format_correctness && !found_node)
				{
					fail_with_message("There is no node with id="
						+ std::to_string(id1) + " for deletion!", line_number);
				}

			}
			else if (line[0] == 'd' && line[1] == 'e') {
				//std::cout << "edge deletion" << std::endl;

				vector<string> x = split(line, ' ');

				if (check_format_correctness && x.size() != 2) fail_with_message("Line with edge deletion must contain 2 elements: 'de ID_E'", line_number);

				try
				{
					id1 = std::stoi(x[1]);
				}
				catch (std::exception& e)
				{
					fail_with_message("Line with edge label change does not use integer ID!", line_number);
				}
				
				bool found_edge = false;
				for (size_t i = 0; i < currentEdges.size(); i++)
				{
					if (currentEdges[i][FR_PE_ID] == id1)
					{
						found_edge = true;
						changelabel = pu.getLabelEncoding("-" + pu.getEncodingLabel(currentEdges[i][FR_PE_LABEL]));
						pu.appendToEncodingLabel(changelabel, "-" + pu.getEncodingLabel(currentEdges[i][FR_PE_LABEL]));

						//std::cout << id1 << " " << id2 << std::endl;
						//std::cout << "LABEL: -, CHANGELABEL: " << changelabel << " " << ("-" + pu.getEncodingLabel(currentEdges[i][FR_PE_LABEL])) << std::endl;

						pu.appendToIsChange(changelabel, true);
						pu.appendToAntecedentLabelEncoding(changelabel, currentEdges[i][FR_PE_LABEL]);

						if (search_for_anomalies)
						{
							pu.appendToDummyLabelEncoding(changelabel, -1);
						}

						currentEdges[i][FR_PE_CHANGELABEL] = changelabel;
						currentEdgesChangetimeRelative[i] = global_snapshot_number - currentEdges[i][FR_PE_CHANGETIME];
						currentEdges[i][FR_PE_CHANGETIME] = global_snapshot_number; 
						currentEdges[i][FR_PE_DELETED] = 1;

						// update changetimelink of the corresponding vertices:
						id2 = currentEdges[i][FR_PE_SRC];
						id3 = currentEdges[i][FR_PE_DST];
						for (size_t j = 0; j < currentNodes.size(); j++)
						{
							if ((currentNodes[j][FR_PN_ID] == id2) || (currentNodes[j][FR_PN_ID] == id3))
							{
								currentNodes[j][FR_PN_CHANGETIMELINK] = global_snapshot_number;
							}
						}

						break;
					}
				}
				if (check_format_correctness && !found_edge)
				{
					fail_with_message("There is no edge with id=" + std::to_string(id1) + " for deletion!", line_number);
				}
			}

		}

		// +1 because graph_number is an index starting from 0
		pu.setNumberOfDynamicGraphs(graph_number + 1);

		return pu;
	}


}