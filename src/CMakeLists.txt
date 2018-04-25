project(src)

SET(HEADER_FILES
        AdjacencyList.h
        DGRMiner.h
        DGRSubgraphMining.h
        FileReader.h
        HelperFunctions.h
        PartialUnion.h
        OverlapGraph.hpp)

set(SOURCE_FILES
        DGRMiner.cpp
        DGRSubgraphMining.cpp
        FileReader.cpp
        HelperFunctions.cpp
        PartialUnion.cpp
        HelperPrintFunctions.cpp
        OverlapGraph.cpp)

add_library(src STATIC ${SOURCE_FILES} ${HEADER_FILES})