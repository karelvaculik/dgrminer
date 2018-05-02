add_executable(runBasicDGRMinerTests
        common_utils.h
        test_common_utils.cpp
        test_dgrsubgraph_mining.cpp
        test_dgrsubgraph_mining_compute_antecedent_pattern_from_pattern.cpp
        test_dgrsubgraph_mining_enumerate.cpp
        test_dgrsubgraph_mining_enumerate_antecedent.cpp
        test_file_reader.cpp
        test_helper_functions.cpp
        test_min_code.cpp
        test_overlaph_graph.cpp
        test_partial_union.cpp)

target_link_libraries(runBasicDGRMinerTests gtest gtest_main)
target_link_libraries(runBasicDGRMinerTests gmock gmock_main)
#target_link_libraries(runBasicDGRMinerTests fmc)
target_link_libraries(runBasicDGRMinerTests src)

file(COPY data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})