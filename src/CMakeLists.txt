project(src)

SET(HEADER_FILES
        AdjacencyList.h
        DGRMiner.h
        DGRSubgraphMining.h
        FileReader.h
        HelperFunctions.h
        PartialUnion.h
        OverlapGraph.hpp
        fmc/findClique.h
        fmc/graphIO.h
        )

set(SOURCE_FILES
        DGRMiner.cpp
        DGRSubgraphMining.cpp
        FileReader.cpp
        HelperFunctions.cpp
        PartialUnion.cpp
        HelperPrintFunctions.cpp
        OverlapGraph.cpp
        fmc/findClique.cpp
        fmc/graphIO.cpp
        fmc/utils.cpp
        fmc/findCliqueHeu.cpp
        )

add_library(src STATIC ${SOURCE_FILES} ${HEADER_FILES})