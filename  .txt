cmake_minimum_required(VERSION 3.9)
project(src)

set(CMAKE_CXX_STANDARD 11)


set(SOURCE_FILES src/main.cpp)
add_executable(dgrminer ${SOURCE_FILES})

include_directories(src)

file(COPY data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(src)
add_subdirectory(test)

target_link_libraries(dgrminer src)