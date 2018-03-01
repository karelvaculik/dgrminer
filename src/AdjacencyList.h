//
// Created by karel on 18.2.18.
//

#ifndef DGRMINER_ADJACENCYLIST_H
#define DGRMINER_ADJACENCYLIST_H

#include "HelperFunctions.h"
#include <vector>
#include <array>

namespace dgrminer
{
    struct AdjacencyListCrate
    {
        std::vector<std::vector<int>> adjacencyList;
        std::vector<std::vector<std::array<int, ADJ_INFO___SIZE>>> adjacencyEdgeInfo;
        // ID, CHANGETIME, LABEL
        std::vector<std::array<int, ADJ_NODES___SIZE>> nodes;
    };
}

#endif //DGRMINER_ADJACENCYLIST_H
