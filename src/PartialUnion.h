//
// Created by karel on 18.2.18.
//

#ifndef DGRMINER_PARTIALUNION_H
#define DGRMINER_PARTIALUNION_H

#include "AdjacencyList.h"
#include "HelperFunctions.h"
#include <vector>
#include <array>
#include <map>
#include <set>

using namespace std;

namespace dgrminer {

    struct labeled_node_time
    {
        int time;
        int id;

        int label;
        int changetime;
    };

    struct labeled_edge_with_occurrences
    {
        // same elements as in AdjacencyInfo
        // src-label, src-changetime, direction, label, changetime, dst-label, dst-changetime, id
        std::array<int, 8> elements;
        mutable std::set<int> occurrences;
    };

    struct labeled_node
    {
        int changetime;
        int label;
        std::set<int> occurrences;

        bool operator () (const labeled_node& ln) const
        {
            return (ln.changetime == changetime && ln.label == label);
        }
    };

    class PartialUnion
    {
        int value;
        std::vector<std::array<int, 6>> nodes;
        std::vector<std::array<int, 8>> edges;
        std::map<std::string, int> labelEncoding;
        std::map<int, std::string> encodingLabel;
        std::map<int, bool> labelIsChange;
        std::map<int, int> antecedentLabelsOfLabels; // label -> antecedent label
        std::map<int, int> dummyLabelsOfLabels;  // label -> dummy label
        int number_of_labels;
        int number_of_dynamic_graphs;

        std::vector<int> mappingSnapshotsToGraphs;

    public:

        PartialUnion();
        ~PartialUnion();
        //void setNodes(std::vector<std::array<int, 7>> & nodes);
        //void setEdges(std::vector<std::array<int, 8>> & edges);
        void appendNode(std::array<int, 6> & node);
        void appendEdge(std::array<int, 8> & edge);
        void printAll();
        void printEncoding();
        void printDimensions();

        int getLabelEncoding(string label);

        int getAntecedentLabel(int label);
        int getAntecedentChangetime(int consequentChangetime);

        int getDummyLabel(int label);

        void appendToMappingSnapshotsToGraphs(int graph_number);
        int queryMappingSnapshotsToGraphs(int snapshot);

        std::string getEncodingLabel(int value);

        void tempAppendToEncoding(string label, int value);
        void appendToEncodingLabel(int encoding, string label);
        void appendToIsChange(int label, bool value);
        void appendToAntecedentLabelEncoding(int label, int oldlabel);
        void appendToDummyLabelEncoding(int label, int oldlabel);
        int getNumberOfSnapshots();
        int getNumberOfNodes();

        void setNumberOfDynamicGraphs(int number_of_graphs);
        int getNumberOfDynamicGraphs();

        void performSimpleTimeAbstraction();
        void performSimpleNodeTimeAbstraction();

        void addLabeledNodeOccurrence(int label, int changetime, std::vector<labeled_node> & labeled_nodes, bool set_of_graphs, int occurrence);

        void computeFrequentVerticesAndRemoveTheInfrequentOnes(int support_as_absolute, double min_confidence, bool compute_confidence, results_crate * results, results_crate_anomalies * results_anomalies,
                                                               int max_absolute_support, bool set_of_graphs, bool search_for_anomalies, double min_anomaly_outlierness);

        std::set<labeled_node_time> compute_labeled_nodes();
        void eraseInvalidEdges(std::set<labeled_node_time> labeled_nodes);
        void compute_labeled_edges(std::vector<std::array<int, 8>> & newedges, std::set<labeled_edge_with_occurrences> & edges_set,
                                   std::set<labeled_node_time> & labeled_nodes, bool debugPrint);
        void removeInfrequentEdges(std::vector<std::array<int, 8>> & newedges, std::set<labeled_edge_with_occurrences> & edges_set,
                                   int support_as_absolute, bool set_of_graphs, bool debugPrint);

        std::vector<AdjacencyListCrate> createAdjacencyLists(std::vector<std::array<int, 8>> & newedges, int snapshots);

        bool matches_the_adj_info_to_antecedent_pattern_edge(const std::array<int, ADJ_INFO___SIZE> &adj_info, const std::array<int, PAT___SIZE> &edge, bool ignore_edge_id);



        void outputEncodingToFile(string filename);

        void debug_printEdges();
        void debug_printNodes();

    };


    void printResultsToFiles(results_crate * results, results_crate_anomalies * results_anomalies, PartialUnion pu, std::string output_file, bool set_of_graphs,
                             bool compute_confidence, bool search_for_anomalies, bool append_to_file);

    inline bool operator<(const labeled_node_time& lhs, const labeled_node_time& rhs)
    {
        return (lhs.time < rhs.time || (lhs.time == rhs.time && lhs.id < rhs.id));
    }
    inline bool operator==(const labeled_node_time& lhs, const labeled_node_time& rhs)
    {
        return (lhs.time == rhs.time &&  lhs.id == rhs.id);
    }
}




#endif //DGRMINER_PARTIALUNION_H
