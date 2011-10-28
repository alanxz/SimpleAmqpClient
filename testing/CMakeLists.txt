include_directories(${GTEST_INCLUDE_DIRS})
include_directories(../src)

add_executable(test_connect test_connect.cpp)
target_link_libraries(test_connect SimpleAmqpClient ${GTEST_BOTH_LIBRARIES})
set_target_properties(test_connect PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_test(test_connect test_connect)
