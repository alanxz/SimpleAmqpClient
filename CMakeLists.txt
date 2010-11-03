CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(SimpleAmqpClient)

#SET(Boost_USE_STATIC_LIBS ON)
#SET(Boost_USE_MULTITHREADED ON)
SET(Boost_DEBUG TRUE)
SET(Boost_DETAILED_FAILURE_MSG TRUE)
SET(BOOST_ROOT /devel/aega/local)
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

    Message.h
    Message.cpp

    Util.h
    Util.cpp

    AmqpResponseLibraryException.h
    AmqpResponseLibraryException.cpp

    AmqpResponseServerException.h
    AmqpResponseServerException.cpp
	)



ADD_LIBRARY(simple-amqp-client-static STATIC ${SAC_LIB_SRCS})
ADD_LIBRARY(simple-amqp-client-shared SHARED ${SAC_LIB_SRCS})

SET_TARGET_PROPERTIES(simple-amqp-client-static PROPERTIES OUTPUT_NAME SimpleAmqpClient)
SET_TARGET_PROPERTIES(simple-amqp-client-shared PROPERTIES OUTPUT_NAME SimpleAmqpClient)

TARGET_LINK_LIBRARIES(simple-amqp-client-shared ${Rabbitmqc_LIBRARY})

ADD_EXECUTABLE(simple_connect examples/simple_connect.cpp)

TARGET_LINK_LIBRARIES(simple_connect simple-amqp-client-shared)
