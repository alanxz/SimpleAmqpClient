project(test_api)

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
    main.cpp
    )
target_link_libraries(test_api SimpleAmqpClient gtest)
set_target_properties(test_api PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
        )
add_test(test_api test_api)
