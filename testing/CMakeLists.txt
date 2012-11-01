include_directories(BEFORE SYSTEM ${GTEST_INCLUDE_DIRS})
include_directories(../src)

add_executable(test_api 
	connected_test.h 
	test_connect.cpp
	test_channels.cpp
	test_exchange.cpp 
	test_queue.cpp 
	test_publish.cpp 
	test_get.cpp 
	test_consume.cpp
  test_message.cpp
  test_table.cpp
	)
target_link_libraries(test_api SimpleAmqpClient ${GTEST_BOTH_LIBRARIES})
add_test(test_api test_api)
