if(PROJECT_IS_TOP_LEVEL)
  set(CMAKE_INSTALL_INCLUDEDIR include/SimpleAmqpClient CACHE PATH "")
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# find_package(<package>) call for consumers to find this project
set(package SimpleAmqpClient)

install(
    DIRECTORY
    include/
    "${PROJECT_BINARY_DIR}/export/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT SimpleAmqpClient_Development
)

install(
    TARGETS SimpleAmqpClient_SimpleAmqpClient
    EXPORT SimpleAmqpClientTargets
    RUNTIME #
    COMPONENT SimpleAmqpClient_Runtime
    LIBRARY #
    COMPONENT SimpleAmqpClient_Runtime
    NAMELINK_COMPONENT SimpleAmqpClient_Development
    ARCHIVE #
    COMPONENT SimpleAmqpClient_Development
    INCLUDES #
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

configure_file(
    cmake/install-config.cmake.in "${package}Config.cmake"
    @ONLY
)

write_basic_package_version_file(
    "${package}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
)

# Allow package maintainers to freely override the path for the configs
set(
    SimpleAmqpClient_INSTALL_CMAKEDIR "${CMAKE_INSTALL_DATADIR}/${package}"
    CACHE PATH "CMake package config location relative to the install prefix"
)
mark_as_advanced(SimpleAmqpClient_INSTALL_CMAKEDIR)

install(
    FILES
    "${PROJECT_BINARY_DIR}/${package}Config.cmake"
    "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
    DESTINATION "${SimpleAmqpClient_INSTALL_CMAKEDIR}"
    COMPONENT SimpleAmqpClient_Development
)

install(
    EXPORT SimpleAmqpClientTargets
    NAMESPACE SimpleAmqpClient::
    DESTINATION "${SimpleAmqpClient_INSTALL_CMAKEDIR}"
    COMPONENT SimpleAmqpClient_Development
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
