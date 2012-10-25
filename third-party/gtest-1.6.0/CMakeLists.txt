project(gtest CXX)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_library(gtest STATIC gtest/gtest-all.cc)
add_library(gtest_main STATIC gtest/gtest_main.cc)

set(GTEST_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
set(GTEST_BOTH_LIBRARIES gtest gtest_main PARENT_SCOPE)
