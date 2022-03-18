cmake_minimum_required(VERSION 3.14)

project(SimpleAmqpClientTests LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

if(PROJECT_IS_TOP_LEVEL)
  find_package(SimpleAmqpClient REQUIRED)
  enable_testing()
endif()

find_package(GTest CONFIG REQUIRED)

add_executable(SimpleAmqpClient_test
  source/test_ack.cpp
  source/test_channels.cpp
  source/test_connect.cpp
  source/test_consume.cpp
  source/test_exchange.cpp
  source/test_get.cpp
  source/test_message.cpp
  source/test_nack.cpp
  source/test_publish.cpp
  source/test_queue.cpp
  source/test_table.cpp
)
target_link_libraries(
    SimpleAmqpClient_test PRIVATE
    SimpleAmqpClient::SimpleAmqpClient
    GTest::gtest_main
)
target_compile_features(SimpleAmqpClient_test PRIVATE cxx_std_17)

add_folders(Test)
