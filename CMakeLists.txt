CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(SimpleAmqpClient)

#SET(Boost_DEBUG TRUE)
#SET(Boost_DETAILED_FAILURE_MSG TRUE)
SET(Boost_ADDITIONAL_VERSIONS "1.47.0")
FIND_PACKAGE(Boost 1.47.0 COMPONENTS chrono system REQUIRED)
INCLUDE_DIRECTORIES(SYSTEM ${Boost_INCLUDE_DIRS})

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Modules)
FIND_PACKAGE(Rabbitmqc REQUIRED)
INCLUDE_DIRECTORIES(SYSTEM ${Rabbitmqc_INCLUDE_DIRS})

INCLUDE_DIRECTORIES(BEFORE src
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

option(BUILD_SHARED_LIBS "Build SimpleAmqpClient as a shared library" ON)

if (WIN32 AND NOT BUILD_SHARED_LIBS)
  message(FATAL_ERROR "The SimpleAmqpClient library cannot be built as a static library on Win32. Set BUILD_SHARED_LIBS=ON to get around this.")
endif()

SET(SAC_LIB_SRCS
	src/SimpleAmqpClient/SimpleAmqpClient.h

	src/SimpleAmqpClient/AmqpException.h
	src/AmqpException.cpp

	src/SimpleAmqpClient/Channel.h
	src/Channel.cpp

	src/SimpleAmqpClient/ChannelImpl.h
	src/ChannelImpl.cpp

	src/SimpleAmqpClient/BasicMessage.h
	src/BasicMessage.cpp

	src/SimpleAmqpClient/Util.h

	src/SimpleAmqpClient/AmqpResponseLibraryException.h
	src/AmqpResponseLibraryException.cpp

	src/SimpleAmqpClient/ConnectionClosedException.h
	src/SimpleAmqpClient/ConsumerTagNotFoundException.h

	src/SimpleAmqpClient/Envelope.h
	src/Envelope.cpp

	src/SimpleAmqpClient/MessageReturnedException.h
	src/MessageReturnedException.cpp
	)


ADD_LIBRARY(SimpleAmqpClient ${SAC_LIB_SRCS})
TARGET_LINK_LIBRARIES(SimpleAmqpClient ${Rabbitmqc_LIBRARY} ${Boost_LIBRARIES} ${SOCKET_LIBRARY})
SET_TARGET_PROPERTIES(SimpleAmqpClient PROPERTIES 
	VERSION 2.0
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib
	)

add_library(SimpleAmqpClient-static STATIC ${SAC_LIB_SRCS})
set_target_properties(SimpleAmqpClient-static PROPERTIES OUTPUT_NAME SimpleAmqpClient)

#ADD_EXECUTABLE(simple_connect examples/simple_connect.cpp)
#TARGET_LINK_LIBRARIES(simple_connect SimpleAmqpClient)
#SET_TARGET_PROPERTIES(simple_connect PROPERTIES
#	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
#
#ADD_EXECUTABLE(consume_with_timeout examples/consume_with_timeout.cpp)
#TARGET_LINK_LIBRARIES(consume_with_timeout SimpleAmqpClient ${Boost_LIBRARIES})
#SET_TARGET_PROPERTIES(consume_with_timeout PROPERTIES
#	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
#
#ADD_EXECUTABLE(envelope_test examples/envelope_test.cpp)
#TARGET_LINK_LIBRARIES(envelope_test SimpleAmqpClient)
#SET_TARGET_PROPERTIES(envelope_test PROPERTIES
#	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
#
#ADD_EXECUTABLE(basic_return_test examples/basic_return_test.cpp)
#TARGET_LINK_LIBRARIES(basic_return_test SimpleAmqpClient)
#SET_TARGET_PROPERTIES(basic_return_test PROPERTIES
#	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

# Some smoke tests:

enable_testing()
find_package(GTest 1.5.0)

if (GTEST_FOUND)
	option(ENABLE_TESTING "Enable smoke tests" ON)
endif (GTEST_FOUND)

if (ENABLE_TESTING)
	add_subdirectory(testing)
endif (ENABLE_TESTING)


# Documentation generation
SET(DOXYFILE_LATEX "NO")
SET(PROJECT_VERSION "2.0.0")
INCLUDE(UseDoxygen)

INSTALL(TARGETS SimpleAmqpClient SimpleAmqpClient-static
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	)

INSTALL(FILES
	src/SimpleAmqpClient/SimpleAmqpClient.h
	src/SimpleAmqpClient/AmqpException.h
	src/SimpleAmqpClient/Channel.h
	src/SimpleAmqpClient/BasicMessage.h
	src/SimpleAmqpClient/Util.h
	src/SimpleAmqpClient/AmqpResponseLibraryException.h
	src/SimpleAmqpClient/ConnectionClosedException.h
	src/SimpleAmqpClient/ConsumerTagNotFoundException.h
	src/SimpleAmqpClient/Envelope.h
	src/SimpleAmqpClient/MessageReturnedException.h
	DESTINATION include/SimpleAmqpClient
	)
