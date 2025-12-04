cmake_minimum_required(VERSION 3.10)
project(jesen VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Library sources
set(JESEN_SOURCES
    jesen_cjson.c
    cJSON/cJSON.c
)

option(JESEN_BUILD_SHARED "Build jesen as a shared library" OFF)

if(JESEN_BUILD_SHARED)
    add_library(jesen SHARED ${JESEN_SOURCES})
    target_compile_definitions(jesen
        PRIVATE JESEN_DLL_EXPORTS
        PUBLIC JESEN_DLL
    )
else()
    add_library(jesen STATIC ${JESEN_SOURCES})
endif()

target_include_directories(jesen
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

# Optional: Build tests
option(JESEN_BUILD_TESTS "Build tests" OFF)

if(JESEN_BUILD_TESTS)
    enable_testing()
    add_executable(test_jesen tests/test_jesen.c)
    target_link_libraries(test_jesen PRIVATE jesen)
    add_test(NAME test_jesen COMMAND test_jesen)
endif()

# Installation
install(TARGETS jesen
    EXPORT jesenTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES jesen.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

set(JESEN_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/jesen")

install(EXPORT jesenTargets
    NAMESPACE jesen::
    DESTINATION ${JESEN_INSTALL_CMAKEDIR}
)

configure_package_config_file(
    cmake/jesenConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/jesenConfig.cmake
    INSTALL_DESTINATION ${JESEN_INSTALL_CMAKEDIR}
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/jesenConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/jesenConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/jesenConfigVersion.cmake
    DESTINATION ${JESEN_INSTALL_CMAKEDIR}
)
