//
// Created by karel on 18.2.18.
//

#ifndef DGRMINER_FILEREADER_H
#define DGRMINER_FILEREADER_H

#include "PartialUnion.h"
#include <map>

namespace dgrminer {



    class FileReader
    {
        //private:
        //	int getLabelEncoding(string label, std::map<std::string, int> & labelEncoding, int * number_of_labels);

    public:
        //FileReader();
        //~FileReader();
        vector<string> split(string str, char delimiter);
        PartialUnion read_dynamic_graph_from_file(std::string filename, int window_size, bool search_for_anomalies);


    };
}




#endif //DGRMINER_FILEREADER_H
