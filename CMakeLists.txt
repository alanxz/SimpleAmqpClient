CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(SimpleAmqpClient)

#SET(Boost_DEBUG TRUE)
#SET(Boost_DETAILED_FAILURE_MSG TRUE)
SET(Boost_ADDITIONAL_VERSIONS "1.44.0" "1.46.1")
FIND_PACKAGE(Boost 1.44.0 COMPONENTS thread REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Modules)
FIND_PACKAGE(Rabbitmqc REQUIRED)
INCLUDE_DIRECTORIES(${Rabbitmqc_INCLUDE_DIRS})

INCLUDE_DIRECTORIES(src
	${CMAKE_CURRENT_BINARY_DIR})

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
	src/SimpleAmqpClient.h

	src/SimpleAmqpClient/Channel.h
	src/Channel.cpp

	src/SimpleAmqpClient/BasicMessage.h
	src/BasicMessage.cpp

	src/SimpleAmqpClient/Util.h
	src/Util.cpp

	src/SimpleAmqpClient/AmqpResponseLibraryException.h
	src/AmqpResponseLibraryException.cpp

	src/SimpleAmqpClient/AmqpResponseServerException.h
	src/AmqpResponseServerException.cpp

	src/SimpleAmqpClient/SimpleRpcClient.h
	src/SimpleRpcClient.cpp

	src/SimpleAmqpClient/SimpleRpcServer.h
	src/SimpleRpcServer.cpp

	src/SimpleAmqpClient/SimplePublisher.h
	src/SimplePublisher.cpp

	src/SimpleAmqpClient/SimpleSubscriber.h
	src/SimpleSubscriber.cpp

	src/SimpleAmqpClient/SimpleTwoWayChannel.h
	src/SimpleTwoWayChannel.cpp
	)



ADD_LIBRARY(SimpleAmqpClient SHARED ${SAC_LIB_SRCS})
TARGET_LINK_LIBRARIES(SimpleAmqpClient ${Rabbitmqc_LIBRARY} ${SOCKET_LIBRARY})
SET_TARGET_PROPERTIES(SimpleAmqpClient PROPERTIES 
	VERSION 1.0
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib
	)


ADD_EXECUTABLE(simple_connect examples/simple_connect.cpp)
TARGET_LINK_LIBRARIES(simple_connect SimpleAmqpClient)
SET_TARGET_PROPERTIES(simple_connect PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

ADD_EXECUTABLE(consume_with_timeout examples/consume_with_timeout.cpp)
TARGET_LINK_LIBRARIES(consume_with_timeout SimpleAmqpClient ${Boost_LIBRARIES})
SET_TARGET_PROPERTIES(consume_with_timeout PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

# Documentation generation
SET(DOXYFILE_LATEX "NO")
SET(PROJECT_VERSION "1.0.0")
INCLUDE(UseDoxygen)

INSTALL(TARGETS SimpleAmqpClient
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib
	)

INSTALL(FILES src/SimpleAmqpClient.h
	DESTINATION include
	)

INSTALL(DIRECTORY src/SimpleAmqpClient/
	DESTINATION include/SimpleAmqpClient
	FILES_MATCHING PATTERN "*.h"
	)
