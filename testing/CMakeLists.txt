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
    test_ack.cpp
    test_nack.cpp
    )
target_include_directories(test_api PRIVATE ${gtest_SOURCE_DIR}/include ${Rabbitmqc_INCLUDE_DIR} "${CMAKE_CURRENT_LIST_DIR}/../src")
target_link_libraries(test_api SimpleAmqpClient gtest gtest_main)
add_test(test_api test_api)
