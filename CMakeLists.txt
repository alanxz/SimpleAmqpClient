CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(SimpleAmqpClient)

# Follow all steps below in order to calculate new ABI version when updating the library
# NOTE: THIS IS UNRELATED to the actual project version
#
# 1. If the library source code has changed at all since the last update, then increment revision
# 2. If any interfaces have been added, removed, or changed since the last update, increment current and set revision to 0.
# 3. If any interfaces have been added since the last public release, then increment age.
# 4. If any interfaces have been removed since the last public release, then set age to 0.

set(SAC_SOVERSION_CURRENT   3)
set(SAC_SOVERSION_REVISION  0)
set(SAC_SOVERSION_AGE       1)

math(EXPR SAC_SOVERSION_MAJOR "${SAC_SOVERSION_CURRENT} - ${SAC_SOVERSION_AGE}")
math(EXPR SAC_SOVERSION_MINOR "${SAC_SOVERSION_AGE}")
math(EXPR SAC_SOVERSION_PATCH "${SAC_SOVERSION_REVISION}")

set(SAC_VERSION ${SAC_SOVERSION_MAJOR}.${SAC_SOVERSION_MINOR}.${SAC_SOVERSION_PATCH})
set(SAC_SOVERSION ${SAC_SOVERSION_MAJOR})

FIND_PACKAGE(Boost 1.47.0 COMPONENTS chrono system REQUIRED)
INCLUDE_DIRECTORIES(SYSTEM ${Boost_INCLUDE_DIRS})

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Modules)
FIND_PACKAGE(Rabbitmqc REQUIRED)
INCLUDE_DIRECTORIES(SYSTEM ${Rabbitmqc_INCLUDE_DIRS})

if (CMAKE_GENERATOR MATCHES ".*(Make|Ninja).*"
    AND NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel" FORCE)
  message(STATUS "CMAKE_BUILD_TYPE not specified. Using ${CMAKE_BUILD_TYPE} build")
endif ()

INCLUDE_DIRECTORIES(BEFORE src
	${CMAKE_CURRENT_BINARY_DIR})

INCLUDE (CheckIncludeFile)

# We need winsock32 if we're compiling on windows
# This needs to match what rabbitmq-c does
if (WIN32)
	set(SOCKET_LIBRARY ws2_32)
  CHECK_INCLUDE_FILE(winsock2.h HAVE_WINSOCK2_H)
else ()
  CHECK_INCLUDE_FILE(sys/socket.h HAVE_SYS_SOCKET_H)
endif ()

INCLUDE (CheckFunctionExists)

CHECK_FUNCTION_EXISTS(strerror_s HAVE_STRERROR_S)
SET(CMAKE_REQUIRED_DEFINITIONS "-D_XOPEN_SOURCE=600")
CHECK_FUNCTION_EXISTS(strerror_r HAVE_STRERROR_R)
UNSET(CMAKE_REQUIRED_DEFINITIONS)

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
 
  src/SimpleAmqpClient/Table.h
  src/Table.cpp
 
  src/SimpleAmqpClient/TableImpl.h
  src/TableImpl.cpp
	)


ADD_LIBRARY(SimpleAmqpClient ${SAC_LIB_SRCS})
TARGET_LINK_LIBRARIES(SimpleAmqpClient ${Rabbitmqc_LIBRARY} ${Boost_LIBRARIES} ${SOCKET_LIBRARY})

if (WIN32)
  set_target_properties(SimpleAmqpClient PROPERTIES VERSION ${SAC_VERSION} OUTPUT_NAME SimpleAmqpClient.${SAC_SOVERSION})
else ()
  set_target_properties(SimpleAmqpClient PROPERTIES VERSION ${SAC_VERSION} SOVERSION ${SAC_SOVERSION})
endif ()


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

option(ENABLE_TESTING "Enable smoke tests" OFF)

if (ENABLE_TESTING)
  enable_testing()
  add_subdirectory(third-party/gtest-1.6.0)
	add_subdirectory(testing)
endif (ENABLE_TESTING)


# Documentation generation
SET(DOXYFILE_LATEX "NO")
SET(PROJECT_VERSION ${SAC_VERSION})
INCLUDE(UseDoxygen)

INSTALL(TARGETS SimpleAmqpClient
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
  src/SimpleAmqpClient/Table.h
	DESTINATION include/SimpleAmqpClient
	)
