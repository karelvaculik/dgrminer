
#include "DGRSubgraphMining.h"

#include <fstream> // temp
#include <sstream>

#include <iostream>
#include <algorithm>

namespace dgrminer
{

    int getDirectionInt(const std::string *direction)
    {
        if (*direction == "u")
        {
            return DIR_UND;
        }
        else
        {
            return DIR_RIGHT;
        }
    }


    int flipDirection(int direction)
    {
        if (direction == DIR_RIGHT)
        {
            return DIR_LEFT;
        }
        else if (direction == DIR_LEFT)
        {
            return DIR_RIGHT;
        }
        else
        {
            return direction;
        }
    }

    // flips edge
    std::array<int, 10> flipPatternEdge(std::array<int, 10> &edge)
    {
        std::array<int, 10> newedge = {edge[1], edge[0],
                                       edge[7], edge[8],
                                       flipDirection(edge[4]), edge[5], edge[6],
                                       edge[2], edge[3],
                                       edge[9]};
        return newedge;
    }

    // flips adjacency info
    std::array<int, 8> flipAdjacencyInfo(std::array<int, 8> &adjacencyinfo)
    {
        // flips src-label with dst-label, src-changetime with dst-changetime, and direction
        std::array<int, 8> newadjacencyinfo = {adjacencyinfo[5], adjacencyinfo[6],
                                               flipDirection(adjacencyinfo[2]), adjacencyinfo[3], adjacencyinfo[4],
                                               adjacencyinfo[0], adjacencyinfo[1], adjacencyinfo[7]};
        return newadjacencyinfo;
    }

    // given an adjacency info, it returs either the info itself or its flipped version (the one that is smaller)
    std::array<int, 8> smallerDirectionOfAdjacencyInfo(std::array<int, 8> &adjacencyinfo)
    {

        std::array<int, 8> flipped = flipAdjacencyInfo(adjacencyinfo);
        for (int i = 0; i < 8; i++)
        {
            if (adjacencyinfo[i] < flipped[i])
            {
                return adjacencyinfo;
            }
            else if (adjacencyinfo[i] > flipped[i])
            {
                return flipped;
            }
        }
        return adjacencyinfo;
    }

    // takes two edges and returns true if edge1 is smaller than edge2
    bool
    is_pattern_edge_smaller_than(const std::array<int, 10> &pattern_edge1, const std::array<int, 10> &pattern_edge2,
                                 bool ignore_edge_id)
    {
        int max_index = (ignore_edge_id) ? 9 : 10;
        bool result = false;
        int i1 = pattern_edge1[0];
        int j1 = pattern_edge1[1];
        int i2 = pattern_edge2[0];
        int j2 = pattern_edge2[1];
        if (i1 < j1) // edge1 is forward
        {
            if (i2 < j2) // edge2 is forward
            {
                result = (j1 < j2) || (i1 > i2 && j1 == j2);
            }
            else // edge2 is backward
            {

                result = j1 <= i2;
            }
        }
        else // edge1 is backward
        {
            if (i2 < j2) // edge2 is forward
            {
                result = i1 < j2;
            }
            else // edge2 is backward
            {
                result = (i1 < i2) || (i1 == i2 && j1 < j2);
            }
        }
        if (!result && i1 == i2 && j1 == j2)
        {
            // check also edge ID (the 10th element)
            for (int k = 2; k < max_index; k++)
            {
                if (pattern_edge1[k] < pattern_edge2[k])
                {
                    result = true;
                    break;
                }
                else if (pattern_edge1[k] > pattern_edge2[k])
                {
                    break;
                }
            }
        }
        return result;
    }

    bool
    is_pattern_edge_equal_to(const std::array<int, 10> &edge1, const std::array<int, 10> &edge2, bool ignore_edge_id)
    {
        int max_index = (ignore_edge_id) ? 9 : 10;
        for (int i = 0; i < max_index; i++)
        {
            if (edge1[i] != edge2[i])
            {
                return false;
            }
        }
        return true;

    }

    bool
    is_code_smaller_than(std::vector<std::array<int, 10>> &edge_list1, std::vector<std::array<int, 10>> &edge_list2,
                         bool ignore_edge_id)
    {
        size_t common_length = (edge_list1.size() < edge_list2.size()) ? edge_list1.size() : edge_list2.size();
        for (size_t i = 0; i < common_length; i++)
        {
            bool intermediate_res = is_pattern_edge_smaller_than(edge_list1[i], edge_list2[i], ignore_edge_id);
            if (intermediate_res)
            {
                return true;
            }
            else
            {
                intermediate_res = is_pattern_edge_equal_to(edge_list1[i], edge_list2[i], ignore_edge_id);
                if (!intermediate_res)
                {
                    return false;
                }
            }
        }
        // if we got here, the prefixes of common length are equal
        // if the first code is shorter, then it is smaller
        return (edge_list1.size() < edge_list2.size());
    }

    bool
    is_adj_info_equal_to(const std::array<int, 8> &adj_info1, const std::array<int, 8> &adj_info2, bool ignore_edge_id)
    {
        int max_index = (ignore_edge_id) ? 7 : 8;
        for (int i = 0; i < max_index; i++)
        {
            if (adj_info1[i] != adj_info2[i])
            {
                return false;
            }
        }
        return true;
    }

    bool is_adj_info_equal_to_pattern_edge(const std::array<int, 8> &adj_info, const std::array<int, 10> &edge,
                                           bool ignore_edge_id)
    {
        int max_index = (ignore_edge_id) ? 7 : 8;
        for (int i = 0; i < max_index; i++)
        {
            if (adj_info[i] != edge[i + 2])
            {
                return false;
            }
        }
        return true;
    }

    // this is just a modification of the is_min_code function
    // edge_list and starting_edges cannot be references - they are modified here
    std::vector<std::array<int, 10>> compute_min_code(std::vector<std::array<int, 10>> edge_list)
    {

        //here we will eventually save the minimum code
        //at the end, we compare it with the current code and decide
        std::vector<std::array<int, 10>> minimum_code;


        int n_edges = edge_list.size();
        for (int i = 0; i < n_edges; i++)
        {
            edge_list.push_back(flipPatternEdge(edge_list[i]));
        }

        std::vector<std::array<int, 8>> starting_edges;

        std::vector<int> indices_of_min_edges = find_minimum_labelings(edge_list, starting_edges, true);

        for (int min_edge_ind : indices_of_min_edges)
        {
            std::array<int, 10> built_min_code_edge = {0, 1, edge_list[min_edge_ind][2], edge_list[min_edge_ind][3],
                                                       edge_list[min_edge_ind][4],
                                                       edge_list[min_edge_ind][5], edge_list[min_edge_ind][6],
                                                       edge_list[min_edge_ind][7], edge_list[min_edge_ind][8],
                                                       edge_list[min_edge_ind][9]};

            // if we already have some min code saved, try to compare the current pattern being built
            // if the current min code is smaller, then just try another init edge
            if (minimum_code.size() > 0 && is_pattern_edge_smaller_than(minimum_code[0], built_min_code_edge, true))
            {
                continue;
            }

            std::vector<std::array<int, 10>> built_min_code;
            built_min_code.push_back(built_min_code_edge);


            std::vector<bool> selected_edges(2 * n_edges, false);
            std::vector<int> selected_edges_ind(n_edges, -1);

            std::vector<int> selected_candidates(n_edges,
                                                 0);  // which candidate was chosen for each edge (used for backtracking)
            // which candidates are possible for each edge (used for backtracking), we are not interested in the 1st element
            std::vector<std::vector<std::array<int, 10>>> all_edge_candidates(n_edges,
                                                                              std::vector<std::array<int, 10>>());
            std::vector<std::vector<int>> all_edge_candidates_selected_edges_ind(n_edges, std::vector<int>());

            selected_edges_ind[0] = min_edge_ind;
            selected_edges[min_edge_ind] = true;  // set the first edge as occupied
            selected_edges[(min_edge_ind + n_edges) %
                           (2 * n_edges)] = true;  // the reversed direction of the edge will be also occupied

            int current_index = 1;

            std::vector<int> src_vertices;
            std::vector<int> dst_vertices;

            // how many edges in built_min_code use the vertices?
            std::map<int, int> amount_of_usage_of_original_vertices;

            // here we keep the mapping of min_code ids to edge_list vertex ids:
            std::map<int, int> vertex_id_mapping;
            vertex_id_mapping[0] = edge_list[min_edge_ind][0];
            vertex_id_mapping[1] = edge_list[min_edge_ind][1];

            for (int i = 0; i < edge_list.size(); i++)
            {
                src_vertices.push_back(edge_list[i][0]);
                dst_vertices.push_back(edge_list[i][1]);
                // all nodes are used 0-times at first
                amount_of_usage_of_original_vertices[edge_list[i][0]] = 0;
                amount_of_usage_of_original_vertices[edge_list[i][1]] = 0;
            }

            // we occupied the first two nodes:
            amount_of_usage_of_original_vertices[edge_list[min_edge_ind][0]] = 1;
            amount_of_usage_of_original_vertices[edge_list[min_edge_ind][1]] = 1;

            // do DFS
            while (true)
            {
                if (current_index >= n_edges)
                {
                    // we generated a code from all edges

                    // we did not generate a smaller code,
                    // try different code (the next one may be even smaller)
                    if (minimum_code.size() == 0)
                    {
                        // just save this first code as minimum
                        minimum_code = built_min_code;
                    }
                    else
                    {
                        // compare it with the latest "minimum code" and if necessary, save the new one:
                        bool is_new_code_smaller = is_code_smaller_than(built_min_code, minimum_code, true);
                        if (is_new_code_smaller)
                        {
                            minimum_code = built_min_code;
                        }
                    }
                    // backtrack
                    current_index--;

                }
                if (current_index == 0)
                {
                    // (we have tried all posibilities from the current initial edge)
                    // we backtracked back to 0, we should start finding the minimum code from other position (break the repeat loop)
                    // (we couldn't find it here)
                    break;
                }

                // we just moved by one edge, find all candidates for growing the the pattern:
                if (selected_candidates[current_index] == 0)
                {
                    std::deque<int> right_most_path = find_rightmost_path(built_min_code);
                    std::deque<int> right_most_path_occurrence;
                    for (int i = 0; i < right_most_path.size(); i++)
                    {
                        right_most_path_occurrence.push_back(vertex_id_mapping[right_most_path[i]]);
                    }
                    int right_most_vertex = right_most_path_occurrence.back();

                    std::vector<labeled_pattern_edge> edge_candidates;

                    // find backward edge candidates:
                    int src = right_most_vertex;
                    for (int i = 0; i < right_most_path.size() - 1; i++)
                    {
                        int dst = right_most_path_occurrence[i];
                        for (int j = 0; j < selected_edges.size(); j++)
                        {
                            if (!selected_edges[j] && src_vertices[j] == src && dst_vertices[j] == dst)
                            {

                                std::array<int, 10> e_cand = {right_most_path.back(), right_most_path[i],
                                                              edge_list[j][2], edge_list[j][3], edge_list[j][4],
                                                              edge_list[j][5], edge_list[j][6], edge_list[j][7],
                                                              edge_list[j][8], edge_list[j][9]};


                                labeled_pattern_edge e_candidate;
                                e_candidate.elements = e_cand;
                                e_candidate.index = j;
                                edge_candidates.push_back(e_candidate);
                            }
                        }
                    }

                    // find forward edge candidates:
                    for (int i = right_most_path.size() - 1; i >= 0; i--)
                    {
                        src = right_most_path_occurrence[i];
                        for (int j = 0; j < selected_edges.size(); j++)
                        {
                            if (!selected_edges[j] && src_vertices[j] == src &&
                                amount_of_usage_of_original_vertices[dst_vertices[j]] == 0)
                            {
                                std::array<int, 10> e_cand = {right_most_path[i], right_most_path.back() + 1,
                                                              edge_list[j][2], edge_list[j][3], edge_list[j][4],
                                                              edge_list[j][5], edge_list[j][6], edge_list[j][7],
                                                              edge_list[j][8], edge_list[j][9]};


                                labeled_pattern_edge e_candidate;
                                e_candidate.elements = e_cand;
                                e_candidate.index = j;
                                edge_candidates.push_back(e_candidate);
                            }
                        }
                    }

                    std::sort(edge_candidates.begin(), edge_candidates.end(), cmp_labeled_pattern_edge);

                    all_edge_candidates[current_index].clear();
                    all_edge_candidates_selected_edges_ind[current_index].clear();
                    for (size_t i = 0; i < edge_candidates.size(); i++)
                    {
                        if (i > 0 &&
                            is_pattern_edge_smaller_than(edge_candidates[i - 1].elements, edge_candidates[i].elements,
                                                         true))
                        {
                            // don't take candidates that cannot have smaller code
                            break;
                        }
                        all_edge_candidates[current_index].push_back(edge_candidates[i].elements);
                        all_edge_candidates_selected_edges_ind[current_index].push_back(edge_candidates[i].index);
                    }
                }


                // try to use the candidates

                bool should_backtrack = true;
                for (int edge_candidate_ind = selected_candidates[current_index];
                     edge_candidate_ind < all_edge_candidates[current_index].size(); edge_candidate_ind++)
                {


                    //if (minimum_code.size() > 0 && is_pattern_edge_smaller_than(minimum_code[current_index], all_edge_candidates[current_index][edge_candidate_ind], true))
                    //{
                    //	// we should backtrack
                    //	break;
                    //}

                    //if (is_pattern_edge_equal_to(all_edge_candidates[current_index][edge_candidate_ind], edge_list[current_index], true))
                    //{
                    // they are same, so append it and move to other edge
                    // NOTE: there can be several edges with the same code (if necessary, these must be tested later as well)
                    selected_candidates[current_index] = edge_candidate_ind + 1;
                    built_min_code.push_back(all_edge_candidates[current_index][edge_candidate_ind]);

                    vertex_id_mapping[all_edge_candidates[current_index][edge_candidate_ind][0]] = edge_list[all_edge_candidates_selected_edges_ind[current_index][edge_candidate_ind]][0];
                    vertex_id_mapping[all_edge_candidates[current_index][edge_candidate_ind][1]] = edge_list[all_edge_candidates_selected_edges_ind[current_index][edge_candidate_ind]][1];

                    amount_of_usage_of_original_vertices[edge_list[all_edge_candidates_selected_edges_ind[current_index][edge_candidate_ind]][0]] += 1;
                    amount_of_usage_of_original_vertices[edge_list[all_edge_candidates_selected_edges_ind[current_index][edge_candidate_ind]][1]] += 1;

                    int sel_e_ind = all_edge_candidates_selected_edges_ind[current_index][edge_candidate_ind];
                    selected_edges_ind[current_index] = sel_e_ind;
                    selected_edges[sel_e_ind] = true;
                    selected_edges[(sel_e_ind + n_edges) % (2 * n_edges)] = true;

                    current_index++;
                    should_backtrack = false;
                    break;
                    //}
                }

                // the candidate edge made larger code or we don't have more candidates to try:
                if (should_backtrack)
                {
                    selected_candidates[current_index] = 0;

                    // now we need to decrement the usage of these two vertices:
                    int decremented1 = vertex_id_mapping[built_min_code.back()[0]];
                    int decremented2 = vertex_id_mapping[built_min_code.back()[1]];
                    amount_of_usage_of_original_vertices[decremented1] -= 1;
                    amount_of_usage_of_original_vertices[decremented2] -= 1;

                    built_min_code.pop_back();

                    int sel_e_ind = selected_edges_ind[current_index];
                    if (sel_e_ind != -1)
                    {
                        selected_edges[sel_e_ind] = false;
                        selected_edges[(sel_e_ind + n_edges) % (2 * n_edges)] = false;
                        selected_edges_ind[current_index] = -1;
                    }

                    current_index--;
                }
            }

        }

        // we could not find a smaller code
        return minimum_code;
    }


    // edge_list and starting_edges cannot be references - they are modified here
    bool is_min_code(std::vector<std::array<int, 10>> edge_list, std::vector<std::array<int, 8>> starting_edges)
    {

        int n_edges = edge_list.size();
        for (int i = 0; i < n_edges; i++)
        {
            edge_list.push_back(flipPatternEdge(edge_list[i]));
        }

        int n_starting_edge = starting_edges.size();
        for (int i = 0; i < n_starting_edge; i++)
        {
            starting_edges.push_back(flipAdjacencyInfo(starting_edges[i]));
        }

        std::vector<int> indices_of_min_edges = find_minimum_labelings(edge_list, starting_edges, false);


        for (int min_edge_ind : indices_of_min_edges)
        {

            std::array<int, 10> built_min_code_edge = {0, 1, edge_list[min_edge_ind][2], edge_list[min_edge_ind][3],
                                                       edge_list[min_edge_ind][4],
                                                       edge_list[min_edge_ind][5], edge_list[min_edge_ind][6],
                                                       edge_list[min_edge_ind][7], edge_list[min_edge_ind][8],
                                                       edge_list[min_edge_ind][9]};

            if (is_pattern_edge_smaller_than(built_min_code_edge, edge_list[0], true))
            {
                return false;
            }

            std::vector<std::array<int, 10>> built_min_code;
            built_min_code.push_back(built_min_code_edge);


            std::vector<bool> selected_edges(2 * n_edges, false);
            std::vector<int> selected_edges_ind(n_edges, -1);

            std::vector<int> selected_candidates(n_edges,
                                                 0);  // which candidate was chosen for each edge (used for backtracking)
            // which candidates are possible for each edge (used for backtracking), we are not interested in the 1st element
            std::vector<std::vector<std::array<int, 10>>> all_edge_candidates(n_edges,
                                                                              std::vector<std::array<int, 10>>());
            std::vector<std::vector<int>> all_edge_candidates_selected_edges_ind(n_edges, std::vector<int>());

            selected_edges_ind[0] = min_edge_ind;
            selected_edges[min_edge_ind] = true;  // set the first edge as occupied
            selected_edges[(min_edge_ind + n_edges) %
                           (2 * n_edges)] = true;  // the reversed direction of the edge will be also occupied

            int current_index = 1;

            std::vector<int> src_vertices;
            std::vector<int> dst_vertices;

            // how many edges in built_min_code use the vertices?
            std::map<int, int> amount_of_usage_of_original_vertices;

            // here we keep the mapping of min_code ids to edge_list vertex ids:
            std::map<int, int> vertex_id_mapping;
            vertex_id_mapping[0] = edge_list[min_edge_ind][0];
            vertex_id_mapping[1] = edge_list[min_edge_ind][1];

            for (int i = 0; i < edge_list.size(); i++)
            {
                src_vertices.push_back(edge_list[i][0]);
                dst_vertices.push_back(edge_list[i][1]);
                // all nodes are used 0-times at first
                amount_of_usage_of_original_vertices[edge_list[i][0]] = 0;
                amount_of_usage_of_original_vertices[edge_list[i][1]] = 0;
            }

            // we occupied the first two nodes:
            amount_of_usage_of_original_vertices[edge_list[min_edge_ind][0]] = 1;
            amount_of_usage_of_original_vertices[edge_list[min_edge_ind][1]] = 1;

            // do DFS
            while (true)
            {
                if (current_index >= n_edges)
                {
                    current_index--;
                }
                if (current_index == 0)
                {
                    // (we have tried all posibilities from the current initial edge)
                    // we backtracked back to 0, we should start finding the minimum code from other position (break the repeat loop)
                    // (we couldn't find it here)
                    break;
                }

                // we just moved by one edge, find all candidates for growing the the pattern:
                if (selected_candidates[current_index] == 0)
                {
                    std::deque<int> right_most_path = find_rightmost_path(built_min_code);
                    std::deque<int> right_most_path_occurrence;
                    for (int i = 0; i < right_most_path.size(); i++)
                    {
                        right_most_path_occurrence.push_back(vertex_id_mapping[right_most_path[i]]);
                    }
                    int right_most_vertex = right_most_path_occurrence.back();

                    std::vector<labeled_pattern_edge> edge_candidates;

                    // find backward edge candidates:
                    int src = right_most_vertex;
                    for (int i = 0; i < right_most_path.size() - 1; i++)
                    {
                        int dst = right_most_path_occurrence[i];
                        for (int j = 0; j < selected_edges.size(); j++)
                        {
                            if (!selected_edges[j] && src_vertices[j] == src && dst_vertices[j] == dst)
                            {
                                std::array<int, 10> e_cand = {right_most_path.back(), right_most_path[i],
                                                              edge_list[j][2], edge_list[j][3], edge_list[j][4],
                                                              edge_list[j][5], edge_list[j][6], edge_list[j][7],
                                                              edge_list[j][8], edge_list[j][9]};
                                labeled_pattern_edge e_candidate;
                                e_candidate.elements = e_cand;
                                e_candidate.index = j;
                                edge_candidates.push_back(e_candidate);
                            }
                        }
                    }

                    // find forward edge candidates:
                    for (int i = right_most_path.size() - 1; i >= 0; i--)
                    {
                        src = right_most_path_occurrence[i];
                        for (int j = 0; j < selected_edges.size(); j++)
                        {
                            if (!selected_edges[j] && src_vertices[j] == src &&
                                amount_of_usage_of_original_vertices[dst_vertices[j]] == 0)
                            {
                                std::array<int, 10> e_cand = {right_most_path[i], right_most_path.back() + 1,
                                                              edge_list[j][2], edge_list[j][3], edge_list[j][4],
                                                              edge_list[j][5], edge_list[j][6], edge_list[j][7],
                                                              edge_list[j][8], edge_list[j][9]};
                                labeled_pattern_edge e_candidate;
                                e_candidate.elements = e_cand;
                                e_candidate.index = j;
                                edge_candidates.push_back(e_candidate);
                            }
                        }
                    }

                    std::sort(edge_candidates.begin(), edge_candidates.end(), cmp_labeled_pattern_edge);

                    all_edge_candidates[current_index].clear();
                    all_edge_candidates_selected_edges_ind[current_index].clear();
                    for (size_t i = 0; i < edge_candidates.size(); i++)
                    {
                        if (i > 0 &&
                            is_pattern_edge_smaller_than(edge_candidates[i - 1].elements, edge_candidates[i].elements,
                                                         true))
                        {
                            // don't take candidates that cannot have smaller code
                            break;
                        }
                        all_edge_candidates[current_index].push_back(edge_candidates[i].elements);
                        all_edge_candidates_selected_edges_ind[current_index].push_back(edge_candidates[i].index);
                    }
                }

                // try to use the candidates
                bool should_backtrack = true;
                for (int edge_candidate_ind = selected_candidates[current_index];
                     edge_candidate_ind < all_edge_candidates[current_index].size(); edge_candidate_ind++)
                {
                    bool is_smaller = is_pattern_edge_smaller_than(
                            all_edge_candidates[current_index][edge_candidate_ind], edge_list[current_index], true);

                    if (is_smaller)
                    {
                        return false;
                    }
                    if (is_pattern_edge_equal_to(all_edge_candidates[current_index][edge_candidate_ind],
                                                 edge_list[current_index], true))
                    {
                        // they are same, so append it and move to other edge
                        // NOTE: there can be several edges with the same code (if necessary, these must be tested later as well)
                        selected_candidates[current_index] = edge_candidate_ind + 1;
                        built_min_code.push_back(all_edge_candidates[current_index][edge_candidate_ind]);

                        vertex_id_mapping[all_edge_candidates[current_index][edge_candidate_ind][0]] = edge_list[all_edge_candidates_selected_edges_ind[current_index][edge_candidate_ind]][0];
                        vertex_id_mapping[all_edge_candidates[current_index][edge_candidate_ind][1]] = edge_list[all_edge_candidates_selected_edges_ind[current_index][edge_candidate_ind]][1];

                        amount_of_usage_of_original_vertices[edge_list[all_edge_candidates_selected_edges_ind[current_index][edge_candidate_ind]][0]] += 1;
                        amount_of_usage_of_original_vertices[edge_list[all_edge_candidates_selected_edges_ind[current_index][edge_candidate_ind]][1]] += 1;

                        int sel_e_ind = all_edge_candidates_selected_edges_ind[current_index][edge_candidate_ind];
                        selected_edges_ind[current_index] = sel_e_ind;
                        selected_edges[sel_e_ind] = true;
                        selected_edges[(sel_e_ind + n_edges) % (2 * n_edges)] = true;

                        current_index++;
                        should_backtrack = false;
                        break;
                    }
                }

                // the candidate edge made larger code or we don't have more candidates to try:
                if (should_backtrack)
                {
                    selected_candidates[current_index] = 0;

                    // now we need to decrement the usage of these two vertices:
                    int decremented1 = vertex_id_mapping[built_min_code.back()[0]];
                    int decremented2 = vertex_id_mapping[built_min_code.back()[1]];
                    amount_of_usage_of_original_vertices[decremented1] -= 1;
                    amount_of_usage_of_original_vertices[decremented2] -= 1;

                    built_min_code.pop_back();

                    int sel_e_ind = selected_edges_ind[current_index];
                    if (sel_e_ind != -1)
                    {
                        selected_edges[sel_e_ind] = false;
                        selected_edges[(sel_e_ind + n_edges) % (2 * n_edges)] = false;
                        selected_edges_ind[current_index] = -1;
                    }

                    current_index--;
                }
            }

        }

        // we could not find a smaller code
        return true;
    }


    inline bool operator<(const labeled_pattern_edge &lhs, const labeled_pattern_edge &rhs)
    {
        return is_pattern_edge_smaller_than(lhs.elements, rhs.elements, false);
    }

    inline bool operator==(const labeled_pattern_edge &lhs, const labeled_pattern_edge &rhs)
    {
        return is_pattern_edge_equal_to(lhs.elements, rhs.elements, false);
    }

    bool cmp_labeled_pattern_edge(const labeled_pattern_edge &lhs, const labeled_pattern_edge &rhs)
    {
        // smallest comes first
        return is_pattern_edge_smaller_than(lhs.elements, rhs.elements, false);
    }


    // find indices of the minimum edge (may have several occurrences) s.t. the minimum edge is also in the starting_edges list
    std::vector<int> find_minimum_labelings(std::vector<std::array<int, 10>> &pattern_edge_list,
                                            std::vector<std::array<int, 8>> &starting_edges,
                                            bool ignore_starting_edges)
    {
        std::array<int, 10> min_edge;
        std::vector<int> min_ind; // can contain several indices, if there are several same edges (probably not necessary to have vector, because it may be impossible to have several of them when using edge IDs)
        for (size_t i = 0; i < pattern_edge_list.size(); i++)
        {
            // check if the i-th edge is in the starting_edges list:
            bool is_in_starting = false;

            if (ignore_starting_edges)
            {
                is_in_starting = true;
            }
            else
            {
                for (size_t j = 0; j < starting_edges.size(); j++)
                {
                    bool is_same = true;

                    // use only the first 7 elements (DO NOT USE edge id)
                    for (int k = 0; k < 7; k++)
                    {
                        if (pattern_edge_list[i][k + 2] != starting_edges[j][k])
                        {
                            is_same = false;
                            break;
                        }
                    }
                    if (is_same)
                    {
                        is_in_starting = true;
                        break;
                    }

                }
            }

            // if it is in the starting_edges list, then
            if (is_in_starting)
            {
                // if it is the first edge, add it
                if (min_ind.size() == 0)
                {
                    min_ind.push_back(i);
                    min_edge = pattern_edge_list[i];
                }
                    // if it is not the first one, check if it is smaller
                else
                {
                    // check also whether it is same as the smallest one
                    bool is_same_as_min = true;
                    // // check also edge ID (10th element)
                    // do not check ID
                    for (int k = 2; k < 9; k++)
                    {
                        if (pattern_edge_list[i][k] < min_edge[k])
                        {
                            min_edge = pattern_edge_list[i];
                            min_ind.clear();
                            min_ind.push_back(i);
                            is_same_as_min = false;
                            break;
                        }
                        else if (pattern_edge_list[i][k] > min_edge[k])
                        {
                            is_same_as_min = false;
                            break;
                        }
                    }
                    if (is_same_as_min)
                    {
                        min_ind.push_back(i);
                    }
                }
            }


        }

        return min_ind;

    }


    // given the pattern code (list of edges), it finds the rightmost path (from root to leaf)
    std::deque<int> find_rightmost_path(std::vector<std::array<int, 10>> &built_min_code)
    {
        std::deque<int> right_most_path;
        int current_value = -1;
        for (int i = built_min_code.size() - 1; i >= 0; i--)
        {
            if (built_min_code[i][0] > built_min_code[i][1])
            {
                // backward edge, skip
                continue;
            }
            // for forward edge do the following:
            if (current_value == -1)
            {
                right_most_path.push_front(built_min_code[i][1]);
                right_most_path.push_front(built_min_code[i][0]);
                current_value = built_min_code[i][0];
            }
            else if (built_min_code[i][1] == current_value)
            {
                right_most_path.push_front(built_min_code[i][0]);
                current_value = built_min_code[i][0];
            }
            if (current_value == 0)
            {
                break;
            }
        }

        return right_most_path;
    }


    void printOutEdges(std::vector<std::array<int, 10>> &edge_list)
    {

        std::cout << "PRINTING EDGE LIST - START" << std::endl;
        for (size_t i = 0; i < edge_list.size(); i++)
        {

            for (int j = 0; j < 10; j++)
            {
                std::cout << edge_list[i][j] << ", ";
            }
            std::cout << std::endl;
        }
        std::cout << "PRINTING EDGE LIST - START" << std::endl;
    }

    void printOutEdges(std::vector<std::array<int, 8>> &edge_list)
    {
        std::cout << "PRINTING EDGE LIST - START" << std::endl;
        for (size_t i = 0; i < edge_list.size(); i++)
        {

            for (int j = 0; j < 8; j++)
            {
                std::cout << edge_list[i][j] << ", ";
            }
            std::cout << std::endl;
        }
        std::cout << "PRINTING EDGE LIST - START" << std::endl;
    }

    void fail_with_message(std::string message, int line_number)
    {
        fail_with_message(message + " (LINE " + std::to_string(line_number) + ")");
    }

    void fail_with_message(std::string message)
    {
        std::cerr << message << std::endl;
        exit(1);
    }


    void debug_output_vector(std::vector<int> vec)
    {
        for (size_t i = 0; i < vec.size(); i++)
        {
            std::cout << vec[i] << ", ";
        }
        std::cout << endl;
    }


    // this is testing function - TEMP
    // FROM: https://stackoverflow.com/questions/14106121/efficient-algorithm-for-finding-all-maximal-subsets
    std::vector<std::vector<int>> find_maximal_subsets(std::vector<std::set<int>> sets)
    {
        std::vector<std::vector<int>> output;
        std::vector<std::vector<std::vector<int>>> buckets;

        // transform sets into vectors:
        std::vector<std::vector<int>> vectors;
        for (size_t i = 0; i < sets.size(); i++)
        {
            std::vector<int> vec;
            for (std::set<int>::iterator j = sets[i].begin(); j != sets[i].end(); j++)
            {
                vec.push_back(*j);
            }
            vectors.push_back(vec);
        }


        if (vectors.size() <= 1)
        { return vectors; }

        // sort the vector by descending length
        std::sort(vectors.begin(), vectors.end(),
                  [](std::vector<int> x, std::vector<int> y) -> int { return (x.size() > y.size()); });


        int length = vectors[0].size();
        std::vector<std::vector<int>> bucket;//current bucket
        //Place each set with shared length in its own bucket
        for (size_t i = 0; i < vectors.size(); i++)
        {
            if (vectors[i].size() == length)//current Bucket
            {
                bucket.push_back(vectors[i]);
            }
            else//new Bucket
            {
                length = vectors[i].size();
                buckets.push_back(bucket);
                std::vector<std::vector<int>> newbucked;
                bucket = newbucked;
                bucket.push_back(vectors[i]);
            }
        }
        buckets.push_back(bucket);

        //Based on the assumption of uniqueness, everything in the first bucket is
        //larger than every other set and since it is unique, they are not proper subsets
        for (size_t i = 0; i < buckets[0].size(); i++)
        {
            output.push_back(buckets[0][i]);
        }

        //Iterate through the buckets
        for (int i = 1; i < buckets.size(); i++)
        {
            std::vector<std::vector<int>> currentBucket = buckets[i];

            //Iterate through the sets in the current bucket
            for (int a = 0; a < currentBucket.size(); a++)
            {
                std::vector<int> currentSet = currentBucket[a]; // this set can be potentially added

                //cout << "CURRENT SET: " << endl;
                //debug_output_vector(currentSet);

                // if the current set is empty and we got here, then there is another set which is a superset of this empty set
                if (currentSet.size() == 0)
                { continue; }

                bool addSet = true;

                //Iterate through buckets with greater length
                for (int b = 0; b < i; b++)
                {
                    std::vector<std::vector<int>> testBucket = buckets[b]; // here can be potentially a super set of the currentSet


                    //Iterate through the sets in testBucket
                    for (int c = 0; c < testBucket.size(); c++)
                    {
                        std::vector<int> testSet = testBucket[c]; // this can be potentially a super set of the currentSet
                        int testMatches = 0;

                        bool setClear = false; // my addition
                        int testIndex = 0;
                        //Iterate through the values in the current set
                        for (int d = 0; d < currentSet.size(); d++)
                        {

                            //Iterate through the values in the test set
                            for (; testIndex < testSet.size(); testIndex++)
                            {
                                if (currentSet[d] < testSet[testIndex])
                                {
                                    setClear = true;
                                    break;
                                }
                                if (currentSet[d] == testSet[testIndex])
                                {
                                    testMatches++;
                                    //cout << "MATCHES: " << testMatches << " ON d = " << d << " AND testIndex = " << testIndex << endl;
                                    if (testMatches == currentSet.size())
                                    {
                                        //cout << "SAME LENGTH" << endl;
                                        addSet = false;
                                        setClear = true;
                                    }
                                    break;
                                }
                            }//testIndex
                            if (setClear)
                            { break; }
                        }//d
                        if (!addSet)
                        { break; }
                    }//c
                    if (!addSet)
                    { break; }
                }//b
                if (addSet)
                { output.push_back(currentSet); }
            }//a
        }//i

        return output;
    }


    // MODIFICATION OF A CODE FROM: https://stackoverflow.com/questions/14106121/efficient-algorithm-for-finding-all-maximal-subsets
    std::vector<non_dummy_vertices_and_edges>
    find_maximal_subsets_of_ndve(std::vector<non_dummy_vertices_and_edges> sets)
    {

        std::vector<non_dummy_vertices_and_edges> output;
        // in regular buckets we keep the sets of edges; they are used in the main algorithm
        std::vector<std::vector<std::vector<int>>> buckets;
        // in buckets_ndve we keep together edges and vertices; these will be used for output
        std::vector<std::vector<non_dummy_vertices_and_edges>> buckets_ndve;


        if (sets.size() <= 1)
        { return sets; }

        // sort the vector by descending length
        std::sort(sets.begin(), sets.end(),
                  [](non_dummy_vertices_and_edges x, non_dummy_vertices_and_edges y) -> int {
                      return (x.non_dummy_edges.size() > y.non_dummy_edges.size());
                  });

        // transform sets into vectors:
        std::vector<std::vector<int>> vectors;
        for (size_t i = 0; i < sets.size(); i++)
        {
            std::vector<int> vec;
            for (std::set<int>::iterator j = sets[i].non_dummy_edges.begin(); j != sets[i].non_dummy_edges.end(); j++)
            {
                vec.push_back(*j);
            }
            vectors.push_back(vec);
        }


        int length = vectors[0].size();
        std::vector<std::vector<int>> bucket; //current bucket
        std::vector<non_dummy_vertices_and_edges> bucket_ndve; //current bucket

        //Place each set with shared length in its own bucket
        for (size_t i = 0; i < vectors.size(); i++)
        {
            if (vectors[i].size() == length)//current Bucket
            {
                bucket.push_back(vectors[i]);
                bucket_ndve.push_back(sets[i]);
            }
            else//new Bucket
            {
                length = vectors[i].size();
                buckets.push_back(bucket);
                buckets_ndve.push_back(bucket_ndve);
                std::vector<std::vector<int>> newbucked;
                std::vector<non_dummy_vertices_and_edges> newbucked_ndve;
                bucket = newbucked;
                bucket_ndve = newbucked_ndve;
                bucket.push_back(vectors[i]);
                bucket_ndve.push_back(sets[i]);
            }
        }
        buckets.push_back(bucket);
        buckets_ndve.push_back(bucket_ndve);

        //Based on the assumption of uniqueness, everything in the first bucket is
        //larger than every other set and since it is unique, they are not proper subsets
        for (size_t i = 0; i < buckets[0].size(); i++)
        {
            //output.push_back(buckets[0][i]);
            output.push_back(buckets_ndve[0][i]);
        }

        //Iterate through the buckets
        for (int i = 1; i < buckets.size(); i++)
        {
            std::vector<std::vector<int>> currentBucket = buckets[i];
            std::vector<non_dummy_vertices_and_edges> currentBucket_ndve = buckets_ndve[i];

            //Iterate through the sets in the current bucket
            for (int a = 0; a < currentBucket.size(); a++)
            {
                std::vector<int> currentSet = currentBucket[a]; // this set can be potentially added
                non_dummy_vertices_and_edges currentSet_ndve = currentBucket_ndve[a];


                // if the current set is empty and we got here, then there is another set which is a superset of this empty set
                if (currentSet.size() == 0)
                { continue; }

                bool addSet = true;

                //Iterate through buckets with greater length
                for (int b = 0; b < i; b++)
                {
                    std::vector<std::vector<int>> testBucket = buckets[b]; // here can be potentially a super set of the currentSet


                    //Iterate through the sets in testBucket
                    for (int c = 0; c < testBucket.size(); c++)
                    {
                        std::vector<int> testSet = testBucket[c]; // this can be potentially a super set of the currentSet
                        int testMatches = 0;

                        bool setClear = false; // my addition
                        int testIndex = 0;
                        //Iterate through the values in the current set
                        for (int d = 0; d < currentSet.size(); d++)
                        {

                            //Iterate through the values in the test set
                            for (; testIndex < testSet.size(); testIndex++)
                            {
                                if (currentSet[d] < testSet[testIndex])
                                {
                                    setClear = true;
                                    break;
                                }
                                if (currentSet[d] == testSet[testIndex])
                                {
                                    testMatches++;
                                    if (testMatches == currentSet.size())
                                    {
                                        // SAME LENGTH
                                        addSet = false;
                                        setClear = true;
                                    }
                                    break;
                                }
                            }//testIndex
                            if (setClear)
                            { break; }
                        }//d
                        if (!addSet)
                        { break; }
                    }//c
                    if (!addSet)
                    { break; }
                }//b
                if (addSet)
                { output.push_back(currentSet_ndve); }
            }//a
        }//i


        return output;
    }

    // MODIFICATION OF A CODE FROM: https://stackoverflow.com/questions/14106121/efficient-algorithm-for-finding-all-maximal-subsets
    std::vector<std::vector<int_pair>> find_maximal_subsets_of_pairs(std::vector<std::set<int_pair>> sets)
    {
        std::vector<std::vector<int_pair>> output;
        std::vector<std::vector<std::vector<int_pair>>> buckets;

        // transform sets into vectors:
        std::vector<std::vector<int_pair>> vectors;
        for (size_t i = 0; i < sets.size(); i++)
        {
            std::vector<int_pair> vec;
            for (std::set<int_pair>::iterator j = sets[i].begin(); j != sets[i].end(); j++)
            {
                vec.push_back(*j);
            }
            vectors.push_back(vec);
        }


        if (vectors.size() <= 1)
        { return vectors; }

        // sort the vector by descending length
        std::sort(vectors.begin(), vectors.end(),
                  [](std::vector<int_pair> x, std::vector<int_pair> y) -> int { return (x.size() > y.size()); });


        int length = vectors[0].size();
        std::vector<std::vector<int_pair>> bucket;//current bucket
        //Place each set with shared length in its own bucket
        for (size_t i = 0; i < vectors.size(); i++)
        {
            if (vectors[i].size() == length)//current Bucket
            {
                bucket.push_back(vectors[i]);
            }
            else//new Bucket
            {
                length = vectors[i].size();
                buckets.push_back(bucket);
                std::vector<std::vector<int_pair>> newbucked;
                bucket = newbucked;
                bucket.push_back(vectors[i]);
            }
        }
        buckets.push_back(bucket);

        //Based on the assumption of uniqueness, everything in the first bucket is
        //larger than every other set and since it is unique, they are not proper subsets
        for (size_t i = 0; i < buckets[0].size(); i++)
        {
            output.push_back(buckets[0][i]);
        }

        //Iterate through the buckets
        for (int i = 1; i < buckets.size(); i++)
        {
            std::vector<std::vector<int_pair>> currentBucket = buckets[i];

            //Iterate through the sets in the current bucket
            for (int a = 0; a < currentBucket.size(); a++)
            {
                std::vector<int_pair> currentSet = currentBucket[a]; // this set can be potentially added


                // if the current set is empty and we got here, then there is another set which is a superset of this empty set
                if (currentSet.size() == 0)
                { continue; }

                bool addSet = true;

                //Iterate through buckets with greater length
                for (int b = 0; b < i; b++)
                {
                    std::vector<std::vector<int_pair>> testBucket = buckets[b]; // here can be potentially a super set of the currentSet


                    //Iterate through the sets in testBucket
                    for (int c = 0; c < testBucket.size(); c++)
                    {
                        std::vector<int_pair> testSet = testBucket[c]; // this can be potentially a super set of the currentSet
                        int testMatches = 0;

                        bool setClear = false; // my addition
                        int testIndex = 0;
                        //Iterate through the values in the current set
                        for (int d = 0; d < currentSet.size(); d++)
                        {

                            //Iterate through the values in the test set
                            for (; testIndex < testSet.size(); testIndex++)
                            {
                                if (currentSet[d] < testSet[testIndex])
                                {
                                    setClear = true;
                                    break;
                                }
                                if (currentSet[d] == testSet[testIndex])
                                {
                                    testMatches++;
                                    if (testMatches == currentSet.size())
                                    {
                                        // SAME LENGTH
                                        addSet = false;
                                        setClear = true;
                                    }
                                    break;
                                }
                            }//testIndex
                            if (setClear)
                            { break; }
                        }//d
                        if (!addSet)
                        { break; }
                    }//c
                    if (!addSet)
                    { break; }
                }//b
                if (addSet)
                { output.push_back(currentSet); }
            }//a
        }//i


        return output;
    }

}