project(fmc)

SET(HEADER_FILES
        findClique.h
        graphIO.h)

set(SOURCE_FILES
        findClique.cpp
        graphIO.cpp
        utils.cpp)

add_library(fmc STATIC ${SOURCE_FILES} ${HEADER_FILES})