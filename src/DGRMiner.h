//
// Created by karel on 18.2.18.
//

#ifndef DGRMINER_DGRMINER_H
#define DGRMINER_DGRMINER_H


#include "PartialUnion.h"

namespace dgrminer
{

    PartialUnion tempReadCompletePartialUnion(void);

    void run_DGRMiner(std::string input_file, std::string output_file, double min_support, double min_confidence,
                      bool compute_confidence, int window_size, std::string str_timeabstraction,
                      bool search_for_anomalies, double min_anomaly_outlierness, bool verbose, bool new_measures, bool heuristic_mis);


    void getFrequentInitialPatterns(std::set<labeled_edge_with_occurrences> & edges_set, std::vector<std::array<int, 8>> & initial_patterns,
                                    std::vector<std::vector<int>> & initial_patterns_occurrences, int support_as_absolute, bool set_of_graphs, PartialUnion &pu);

}
#endif //DGRMINER_DGRMINER_H
