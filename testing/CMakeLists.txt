include_directories(${GTEST_INCLUDE_DIRS})
include_directories(../src)

add_executable(test_connect test_connect.cpp)
target_link_libraries(test_connect SimpleAmqpClient ${GTEST_BOTH_LIBRARIES})
set_target_properties(test_connect PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_test(test_connect test_connect)

add_executable(test_channels test_channels.cpp)
target_link_libraries(test_channels SimpleAmqpClient ${GTEST_BOTH_LIBRARIES})
set_target_properties(test_channels PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_test(test_channels test_channels)

add_executable(test_publish test_publish.cpp)
target_link_libraries(test_publish SimpleAmqpClient ${GTEST_BOTH_LIBRARIES})
set_target_properties(test_publish PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_test(test_publish test_publish)

add_executable(test_get test_get.cpp)
target_link_libraries(test_get SimpleAmqpClient ${GTEST_BOTH_LIBRARIES})
set_target_properties(test_get PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_test(test_get test_get)

add_executable(test_consume test_consume.cpp)
target_link_libraries(test_consume SimpleAmqpClient ${GTEST_BOTH_LIBRARIES})
set_target_properties(test_consume PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_test(test_consume test_consume)

add_executable(test_exchange connected_test.h test_exchange.cpp test_queue.cpp)
target_link_libraries(test_exchange SimpleAmqpClient ${GTEST_BOTH_LIBRARIES})
set_target_properties(test_exchange PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_test(test_exchange test_exchange)