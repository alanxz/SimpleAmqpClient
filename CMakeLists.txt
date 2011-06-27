CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(SimpleAmqpClient)

#SET(Boost_DEBUG TRUE)
#SET(Boost_DETAILED_FAILURE_MSG TRUE)
SET(Boost_ADDITIONAL_VERSIONS "1.44.0" "1.46.1")

FIND_PACKAGE(Boost 1.44.0 COMPONENTS thread REQUIRED)

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Modules)
FIND_PACKAGE(Rabbitmqc REQUIRED)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS}
					${Rabbitmqc_INCLUDE_DIRS}
					${CMAKE_CURRENT_SOURCE_DIR}
					${CMAKE_CURRENT_BINARY_DIR})

ADD_DEFINITIONS(-DBUILDING_SIMPLEAMQPCLIENT)

# We need winsock32 if we're compiling on windows
IF (WIN32)
	SET(SOCKET_LIBRARY ws2_32)
ENDIF()

INCLUDE (CheckIncludeFile)
INCLUDE (CheckFunctionExists)

CHECK_FUNCTION_EXISTS(strerror_s HAVE_STRERROR_S)
SET(CMAKE_REQUIRED_DEFINITIONS "-D_XOPEN_SOURCE=600")
CHECK_FUNCTION_EXISTS(strerror_r HAVE_STRERROR_R)
UNSET(CMAKE_REQUIRED_DEFINITIONS)

CHECK_INCLUDE_FILE(winsock2.h HAVE_WINSOCK2_H)
CHECK_INCLUDE_FILE(sys/socket.h HAVE_SYS_SOCKET_H)

CONFIGURE_FILE(config.h.in config.h)

SET(SAC_LIB_SRCS
	SimpleAmqpClient.h

    Channel.h
    Channel.cpp

    BasicMessage.h
    BasicMessage.cpp

    Util.h
    Util.cpp

    AmqpResponseLibraryException.h
    AmqpResponseLibraryException.cpp

    AmqpResponseServerException.h
    AmqpResponseServerException.cpp

	SimpleRpcClient.h
	SimpleRpcClient.cpp

	SimpleRpcServer.h
	SimpleRpcServer.cpp

	SimplePublisher.h
	SimplePublisher.cpp

	SimpleSubscriber.h
	SimpleSubscriber.cpp

	SimpleTwoWayChannel.h
	SimpleTwoWayChannel.cpp
	)



ADD_LIBRARY(simple-amqp-client-static STATIC ${SAC_LIB_SRCS})
ADD_LIBRARY(simple-amqp-client-shared SHARED ${SAC_LIB_SRCS})

SET_TARGET_PROPERTIES(simple-amqp-client-static PROPERTIES OUTPUT_NAME SimpleAmqpClient)
SET_TARGET_PROPERTIES(simple-amqp-client-shared PROPERTIES 
	OUTPUT_NAME SimpleAmqpClient
	VERSION 1.0)

TARGET_LINK_LIBRARIES(simple-amqp-client-shared ${Rabbitmqc_LIBRARY} ${SOCKET_LIBRARY})

ADD_EXECUTABLE(simple_connect examples/simple_connect.cpp)

TARGET_LINK_LIBRARIES(simple_connect simple-amqp-client-shared)

ADD_EXECUTABLE(consume_with_timeout examples/consume_with_timeout.cpp)

TARGET_LINK_LIBRARIES(consume_with_timeout simple-amqp-client-shared ${Boost_LIBRARIES})

# Documentation generation
SET(DOXYFILE_LATEX "NO")
SET(PROJECT_VERSION "1.0.0")
INCLUDE(UseDoxygen)
