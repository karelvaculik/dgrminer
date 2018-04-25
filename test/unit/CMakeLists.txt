add_executable(runBasicDGRMinerTests
        test_min_code.cpp
        common_utils.h
        test_common_utils.cpp
        test_dgrsubgraph_mining.cpp
        test_helper_functions.cpp
        test_dgrsubgraph_mining_enumerate.cpp
        test_overlaph_graph.cpp)

target_link_libraries(runBasicDGRMinerTests gtest gtest_main)
target_link_libraries(runBasicDGRMinerTests gmock gmock_main)
target_link_libraries(runBasicDGRMinerTests fmc)
target_link_libraries(runBasicDGRMinerTests src)
