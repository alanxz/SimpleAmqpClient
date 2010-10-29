CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(SimpleAmqpClient)

SET(Boost_USE_STATIC_LIBS ON)
SET(Boost_USE_MULTITHREADED ON)
SET(Boost_DETAILED_FAILURE_MSG ON)
SET(Boost_ADDITIONAL_VERSIONS "1.44.0")

FIND_PACKAGE(Boost 1.44.0)

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Modules)
FIND_PACKAGE(Rabbitmqc)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS}
					${Rabbitmqc_INCLUDE_DIRS}
					${CMAKE_CURRENT_SOURCE_DIR})

SET(SAC_LIB_SRCS
    Connection.h
    Connection.cpp

    Channel.h
    Channel.cpp


	)


ADD_LIBRARY(simple-amqp-client-static STATIC ${SAC_LIB_SRCS})
ADD_LIBRARY(simple-amqp-client-shared SHARED ${SAC_LIB_SRCS})

SET_TARGET_PROPERTIES(simple-amqp-client-static PROPERTIES OUTPUT_NAME SimpleAmqpClient)
SET_TARGET_PROPERTIES(simple-amqp-client-shared PROPERTIES OUTPUT_NAME SimpleAmqpClient)

TARGET_LINK_LIBRARIES(simple-amqp-client-shared ${Rabbitmqc_LIBRARY})

