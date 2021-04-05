CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(SimpleAmqpClient)

# Follow all steps below in order to calculate new ABI version when updating the library
# NOTE: THIS IS UNRELATED to the actual project version
#
# 1. If the library source code has changed at all since the last update, then increment revision
# 2. If any interfaces have been added, removed, or changed since the last update, increment
#    current and set revision to 0.
# 3. If any interfaces have been added since the last public release, then increment age.
# 4. If any interfaces have been removed since the last public release, then set age to 0.

set(SAC_SOVERSION_CURRENT   6)
set(SAC_SOVERSION_REVISION  0)
set(SAC_SOVERSION_AGE       4)

math(EXPR SAC_SOVERSION_MAJOR "${SAC_SOVERSION_CURRENT} - ${SAC_SOVERSION_AGE}")
math(EXPR SAC_SOVERSION_MINOR "${SAC_SOVERSION_AGE}")
math(EXPR SAC_SOVERSION_PATCH "${SAC_SOVERSION_REVISION}")

set(SAC_VERSION ${SAC_SOVERSION_MAJOR}.${SAC_SOVERSION_MINOR}.${SAC_SOVERSION_PATCH})
set(SAC_SOVERSION ${SAC_SOVERSION_MAJOR})

file(STRINGS src/SimpleAmqpClient/Version.h _API_VERSION_MAJOR REGEX "^#define SIMPLEAMQPCLIENT_VERSION_MAJOR [0-9]+$")
file(STRINGS src/SimpleAmqpClient/Version.h _API_VERSION_MINOR REGEX "^#define SIMPLEAMQPCLIENT_VERSION_MINOR [0-9]+$")
file(STRINGS src/SimpleAmqpClient/Version.h _API_VERSION_PATCH REGEX "^#define SIMPLEAMQPCLIENT_VERSION_PATCH [0-9]+$")

string(REGEX MATCH "[0-9]+" _API_VERSION_MAJOR ${_API_VERSION_MAJOR})
string(REGEX MATCH "[0-9]+" _API_VERSION_MINOR ${_API_VERSION_MINOR})
string(REGEX MATCH "[0-9]+" _API_VERSION_PATCH ${_API_VERSION_PATCH})

set(SAC_APIVERSION ${_API_VERSION_MAJOR}.${_API_VERSION_MINOR}.${_API_VERSION_PATCH})

#SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Modules)
#FIND_PACKAGE(CONAN_PKG::rabbitmq-c REQUIRED)
#INCLUDE_DIRECTORIES(SYSTEM ${Rabbitmqc_INCLUDE_DIRS})

option(ENABLE_TESTING "Enable tests" OFF)
option(ENABLE_DOC "Enable doxygen docs" OFF)
option(ENABLE_SSL_SUPPORT "Enable SSL support." ${Rabbitmqc_SSL_ENABLED})
option(BUILD_SHARED_LIBS "Build SimpleAmqpClient as a shared library" ON)
message(STATUS "SSL is " ${ENABLE_SSL_SUPPORT})

if (ENABLE_SSL_SUPPORT)
  add_definitions(-DSAC_SSL_SUPPORT_ENABLED)
endif()

if (CMAKE_GENERATOR MATCHES ".*(Make|Ninja).*" AND NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel" FORCE)
  message(STATUS "CMAKE_BUILD_TYPE not specified. Using ${CMAKE_BUILD_TYPE} build")
endif ()

if (CMAKE_CXX_FLAGS STREQUAL ""
    AND NOT DEFINED SAC_CXX_FLAGS_SET)
  if (CMAKE_COMPILER_IS_GNUCXX
      OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    SET(CMAKE_CXX_FLAGS "-Wall -Wextra" CACHE STRING "Flags used by the compiler during all build types." FORCE)
  endif ()
  set(SAC_CXX_FLAGS_SET TRUE CACHE INTERNAL "Have the SAC default compiler flags been set?")
endif ()

if (WIN32)
    set(SOCKET_LIBRARY ws2_32)
endif ()

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

    src/SimpleAmqpClient/AmqpLibraryException.h
    src/AmqpLibraryException.cpp

    src/SimpleAmqpClient/AmqpResponseLibraryException.h
    src/AmqpResponseLibraryException.cpp

    src/SimpleAmqpClient/BadUriException.h
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

ADD_LIBRARY(SimpleAmqpClient STATIC ${SAC_LIB_SRCS})
find_package(rabbitmq-c)
TARGET_LINK_LIBRARIES(SimpleAmqpClient rabbitmq::rabbitmq-static ${SOCKET_LIBRARY})

set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
        )

target_include_directories(SimpleAmqpClient PUBLIC src)

if(WIN32)
  set_target_properties(SimpleAmqpClient PROPERTIES VERSION ${SAC_VERSION} OUTPUT_NAME SimpleAmqpClient.${SAC_SOVERSION})
else ()
  set_target_properties(SimpleAmqpClient PROPERTIES VERSION ${SAC_VERSION} SOVERSION ${SAC_SOVERSION})
endif ()

if(ENABLE_TESTING)
    enable_testing()

    set(BUILD_GTEST ON)
    set(BUILD_GMOCK OFF)

    # This only affects targets declared after this.
    set(BUILD_SHARED_LIBS OFF)

    mark_as_advanced(BUILD_GMOCK)
    mark_as_advanced(BUILD_GTEST)
    mark_as_advanced(gmock_build_tests)
    mark_as_advanced(gtest_build_samples)
    mark_as_advanced(gtest_build_tests)
    mark_as_advanced(gtest_disable_pthreads)
    mark_as_advanced(gtest_force_shared_crt)
    mark_as_advanced(gtest_hide_internal_symbols)

    add_subdirectory(third-party/googletest)
    add_subdirectory(testing)
endif()

# Documentation generation
if(ENABLE_DOC)
    SET(DOXYFILE_LATEX "NO")
    SET(PROJECT_VERSION ${SAC_VERSION})
    INCLUDE(UseDoxygen)
endif()

INSTALL(TARGETS SimpleAmqpClient
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    )

INSTALL(FILES
    src/SimpleAmqpClient/AmqpException.h
    src/SimpleAmqpClient/AmqpLibraryException.h
    src/SimpleAmqpClient/AmqpResponseLibraryException.h
    src/SimpleAmqpClient/BadUriException.h
    src/SimpleAmqpClient/BasicMessage.h
    src/SimpleAmqpClient/Channel.h
    src/SimpleAmqpClient/ConnectionClosedException.h
    src/SimpleAmqpClient/ConsumerCancelledException.h
    src/SimpleAmqpClient/ConsumerTagNotFoundException.h
    src/SimpleAmqpClient/Envelope.h
    src/SimpleAmqpClient/MessageReturnedException.h
    src/SimpleAmqpClient/SimpleAmqpClient.h
    src/SimpleAmqpClient/Table.h
    src/SimpleAmqpClient/Util.h
    src/SimpleAmqpClient/Version.h
    DESTINATION include/SimpleAmqpClient
    )

set(prefix ${CMAKE_INSTALL_PREFIX})
set(exec_prefix "\${prefix}")
set(libdir "\${exec_prefix}/lib")
set(includedir "\${prefix}/include")

configure_file(libSimpleAmqpClient.pc.in ${CMAKE_CURRENT_BINARY_DIR}/libSimpleAmqpClient.pc @ONLY)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/libSimpleAmqpClient.pc
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig
    )
