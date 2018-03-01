//
// Created by karel on 18.2.18.
//

#ifndef DGRMINER_HELPERFUNCTIONS_H
#define DGRMINER_HELPERFUNCTIONS_H


//#include "PartialUnion.h"
#include <string>
#include <array>
#include <vector>
#include <deque>
#include <set>
#include <iostream>

namespace dgrminer {

    const bool DEBUG = false;

    // constants used for FileFeader
    // nodes
    const int FR_PN_TIME = 0;
    const int FR_PN_ID = 1;
    const int FR_PN_LABEL = 2;
    const int FR_PN_DELETED = 3;
    const int FR_PN_CHANGETIME = 4;
    const int FR_PN_CHANGETIMELINK = 5;
    const int FR_PN_CHANGELABEL = 6;

    const int FR_PN___SIZE = 7;

    // edges
    const int FR_PE_TIME = 0;
    const int FR_PE_SRC = 1;
    const int FR_PE_DST = 2;
    const int FR_PE_LABEL = 3;
    const int FR_PE_DIRECTION = 4;
    const int FR_PE_DELETED = 5;
    const int FR_PE_CHANGETIME = 6;
    const int FR_PE_CHANGELABEL = 7;
    const int FR_PE_ID = 8;

    const int FR_PE___SIZE = 9;

    // constants used for PartialUnion
    // nodes
    const int PN_TIME = 0;
    const int PN_ID = 1;
    const int PN_LABEL = 2;
    const int PN_DELETED = 3;
    const int PN_CHANGETIME = 4;
    const int PN_CHANGETIMELINK = 5;

    const int PN___SIZE = 6;


    // edges
    const int PE_TIME = 0;
    const int PE_SRC = 1;
    const int PE_DST = 2;
    const int PE_LABEL = 3;
    const int PE_DIRECTION = 4;
    const int PE_DELETED = 5;
    const int PE_CHANGETIME = 6;
    const int PE_ID = 7;

    const int PE___SIZE = 8;

    // constants used for adjacencyEdgeInfo
    // src-label, src-changetime, direction, label, changetime, dst-label, dst-changetime, id
    const int ADJ_INFO_SRC_LABEL = 0;
    const int ADJ_INFO_SRC_CHANGETIME = 1;
    const int ADJ_INFO_DIRECTION = 2;
    const int ADJ_INFO_LABEL = 3;
    const int ADJ_INFO_CHANGETIME = 4;
    const int ADJ_INFO_DST_LABEL = 5;
    const int ADJ_INFO_DST_CHANGETIME = 6;
    const int ADJ_INFO_ID = 7;

    const int ADJ_INFO___SIZE = 8;

    // constants used for adjacencyEdgeInfo
    const int ADJ_NODES_ID = 0;
    const int ADJ_NODES_CHANGETIME = 1;
    const int ADJ_NODES_LABEL = 2;

    const int ADJ_NODES___SIZE = 3;

    // constants for pattern edges (2 ID elements + adjacencyEdgeInfo elements)
    const int PAT_SRC_ID = 0;
    const int PAT_DST_ID = 1;
    const int PAT_SRC_LABEL = 2;
    const int PAT_SRC_CHANGETIME = 3;
    const int PAT_DIRECTION = 4;
    const int PAT_LABEL = 5;
    const int PAT_CHANGETIME = 6;
    const int PAT_DST_LABEL = 7;
    const int PAT_DST_CHANGETIME = 8;
    const int PAT_ID = 9;  // ID must be last

    const int PAT___SIZE = 10;


    // constants used for direction
    const int DIR_UND = 0;
    const int DIR_RIGHT = 1;
    const int DIR_LEFT = 2;

    // for results:

    const int RESULT_NODES___SIZE = 4; // pattern ID, node ID, node LAB, node CHANGETIME
    const int RESULT_EDGES___SIZE = 6; // pattern ID, src ID, dst ID, LABEL, DIR, CHANGETIME

    // other constants:
    const int ANTECEDENT_LABEL_OF_ADDITION = -1;


    // used in is_min_code();
    struct labeled_pattern_edge
    {
        std::array<int, 10> elements;
        //mutable std::vector<int> occurrences;
        int index;
    };


    struct results_crate
    {
        // FOR FREQUENT PATTERNS:
        int saved_instances;
        std::vector<std::array<int, RESULT_NODES___SIZE>> result_nodes;
        std::vector<std::array<int, RESULT_EDGES___SIZE>> result_edges;
        std::vector<int> support_absolute;
        std::vector<double> support;
        std::vector<double> confidence;
        std::vector<std::vector<int>> occurrences;
        std::vector<std::vector<int>> occurrences_antecedent;


    };

    struct results_crate_anomalies
    {
        // FOR ANOMALIES:
        int anomaly_saved_instances;
        std::vector<std::array<int, RESULT_NODES___SIZE>> anomaly_result_nodes;
        std::vector<std::array<int, RESULT_EDGES___SIZE>> anomaly_result_edges;
        // these two vectors must have the same length and they serve as mapping from anomalies to explanatory patterns:
        std::vector<int> anomaly_id_of_anomalous_pattern; // here we have the IDS of anomaly patterns: 1, 2, 3, 4, ...
        std::vector<int> anomaly_id_of_explanation_pattern; // here we have the IDS of frequent patterns which explain corresponding anomalies, e.g.: 1, 1, 1, 2, 3, 3, ...
        std::vector<double> anomaly_outlierness;
        std::vector<std::vector<int>> anomaly_occurrences;
    };


    int getDirectionInt(const std::string * direction);
    int flipDirection(int direction);

    std::array<int, PAT___SIZE> flipPatternEdge(std::array<int, PAT___SIZE> & edge);
    std::array<int, ADJ_INFO___SIZE> flipAdjacencyInfo(std::array<int, ADJ_INFO___SIZE> & adjacencyinfo);
    std::array<int, ADJ_INFO___SIZE> smallerDirectionOfAdjacencyInfo(std::array<int, ADJ_INFO___SIZE> & adjacencyinfo);

    bool is_pattern_edge_smaller_than(const std::array<int, PAT___SIZE> &edge1, const std::array<int, PAT___SIZE> &edge2, bool ignore_edge_id);
    bool is_pattern_edge_equal_to(const std::array<int, PAT___SIZE> &edge1, const std::array<int, PAT___SIZE> &edge2, bool ignore_edge_id);

    bool is_code_smaller_than(std::vector<std::array<int, 10>> &edge_list1, std::vector<std::array<int, 10>> &edge_list2, bool ignore_edge_id);

    bool is_adj_info_equal_to(const std::array<int, ADJ_INFO___SIZE> &adj_info1, const std::array<int, ADJ_INFO___SIZE> &adj_info2, bool ignore_edge_id);
    bool is_adj_info_equal_to_pattern_edge(const std::array<int, ADJ_INFO___SIZE> &adj_info, const std::array<int, PAT___SIZE> &edge, bool ignore_edge_id);

    bool is_min_code(std::vector<std::array<int, PAT___SIZE>> edge_list, std::vector<std::array<int, 8>> starting_edges);

    std::vector<std::array<int, 10>> compute_min_code(std::vector<std::array<int, 10>> edge_list);

    bool cmp_labeled_pattern_edge(const labeled_pattern_edge& lhs, const labeled_pattern_edge& rhs);

    std::vector<int> find_minimum_labelings(std::vector<std::array<int, 10>> &edge_list, std::vector<std::array<int, 8>> &starting_edges, bool ignore_starting_edges);

    std::deque<int> find_rightmost_path(std::vector<std::array<int, 10>> &built_min_code);


    void fail_with_message(std::string message, int line_number);
    void fail_with_message(std::string message);

    // tmp functions / debug functions
    void printOutEdges(std::vector<std::array<int, 10>> &edge_list);
    void printOutEdges(std::vector<std::array<int, 8>> &edge_list);


    struct int_pair
    {
        int first;
        int second;

        bool operator () (const int_pair& ln) const
        {
            return (ln.first == first && ln.second == second);
        }
    };

    inline bool operator<(const int_pair& lhs, const int_pair& rhs)
    {
        return (lhs.first < rhs.first || (lhs.first == rhs.first && lhs.second < rhs.second));
    }
    inline bool operator==(const int_pair& lhs, const int_pair& rhs)
    {
        return (lhs.first == rhs.first &&  lhs.second == rhs.second);
    }


    // used when searching for addition part of the anomaly patterns
    // comparison is done only on edges:
    struct non_dummy_vertices_and_edges
    {
        std::set<int> non_dummy_vertices;
        std::set<int> non_dummy_edges;
    };
    inline bool operator<(const non_dummy_vertices_and_edges& lhs, const non_dummy_vertices_and_edges& rhs)
    {
        return lhs.non_dummy_edges < rhs.non_dummy_edges;
    }
    inline bool operator==(const non_dummy_vertices_and_edges& lhs, const non_dummy_vertices_and_edges& rhs)
    {
        return lhs.non_dummy_edges == rhs.non_dummy_edges;
    }


    std::vector<std::vector<int>> find_maximal_subsets(std::vector<std::set<int>> sets);
    std::vector<non_dummy_vertices_and_edges> find_maximal_subsets_of_ndve(std::vector<non_dummy_vertices_and_edges> sets);
    std::vector<std::vector<int_pair>> find_maximal_subsets_of_pairs(std::vector<std::set<int_pair>> sets);



    struct anomaly_pattern_with_occurrences
    {
        std::vector<std::array<int, PAT___SIZE>> anomalous_pattern_edges;
        mutable std::set<int> occurrences;
        // here we save the minimum code of the pattern
        // it is then used to find same patterns
        std::vector<std::array<int, PAT___SIZE>> minimum_code;
    };

    // it assumes that lhs and rhs are both of equal size
    inline bool operator<(const anomaly_pattern_with_occurrences& lhs, const anomaly_pattern_with_occurrences& rhs)
    {
        // (ignoring edge id)
        //return is_pattern_edge_smaller_than(lhs.elements, rhs.elements, true);
        for (size_t i = 0; i < lhs.minimum_code.size(); i++)
        {
            bool intermediate_res = is_pattern_edge_smaller_than(lhs.minimum_code[i], rhs.minimum_code[i], true);
            if (intermediate_res)
            {
                return true;
            }
            else
            {
                intermediate_res = is_pattern_edge_equal_to(lhs.minimum_code[i], rhs.minimum_code[i], true);
                if (!intermediate_res)
                {
                    return false;
                }
            }
        }
        return false;
    }

    // it assumes that lhs and rhs are both of equal size
    inline bool operator==(const anomaly_pattern_with_occurrences& lhs, const anomaly_pattern_with_occurrences& rhs)
    {
        // (ignoring edge id)
        //return is_pattern_edge_equal_to(lhs.elements, rhs.elements, true);
        for (size_t i = 0; i < lhs.minimum_code.size(); i++)
        {
            bool intermediate_res = is_pattern_edge_equal_to(lhs.minimum_code[i], rhs.minimum_code[i], true);
            if (!intermediate_res)
            {
                return false;
            }
        }
        return true;
    }
}


#endif //DGRMINER_HELPERFUNCTIONS_H
