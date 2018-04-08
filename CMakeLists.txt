cmake_minimum_required(VERSION 3.9)
project(mm_gentext)

set(CMAKE_CXX_STANDARD 14)

add_executable(mm_gentext main.cpp)

find_package(GTest)

include(GoogleTest)
include_directories(src)

# TODO Rename
add_executable(mm_gentext_tests tests/markov_chain_tests.cpp)
target_link_libraries(mm_gentext_tests ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY} pthread)