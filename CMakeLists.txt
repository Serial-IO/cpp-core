cmake_minimum_required(VERSION 3.30)

# Export compile commands to root directory
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---------------------------------------------------------------------------
# Version information from Git tags
# Version is determined automatically from Git tags and cannot be overridden
# ---------------------------------------------------------------------------
include(${CMAKE_CURRENT_LIST_DIR}/cmake/get_version_from_git.cmake)

# Get version from Git tags
get_version_from_git()
set(CPP_CORE_VERSION_MAJOR ${VERSION_MAJOR} CACHE STRING "Major version number" FORCE)
set(CPP_CORE_VERSION_MINOR ${VERSION_MINOR} CACHE STRING "Minor version number" FORCE)
set(CPP_CORE_VERSION_PATCH ${VERSION_PATCH} CACHE STRING "Patch version number" FORCE)
set(CPP_CORE_VERSION_STRING ${VERSION_STRING} CACHE STRING "Full version string" FORCE)

# Set PROJECT_VERSION for CMake package config (only MAJOR.MINOR.PATCH, no suffix)
set(PROJECT_VERSION "${CPP_CORE_VERSION_MAJOR}.${CPP_CORE_VERSION_MINOR}.${CPP_CORE_VERSION_PATCH}")

project(cpp-core
    VERSION ${PROJECT_VERSION}
    DESCRIPTION "Cross-platform helper library shared by cpp-linux-bindings and cpp-windows-bindings"
    LANGUAGES CXX)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp_core/version.h.in
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp_core/version.h
    @ONLY)

# Header-only library --------------------------------------------------------
add_library(cpp_core INTERFACE)
add_library(cpp_core::cpp_core ALIAS cpp_core)

# Public include directories
target_include_directories(cpp_core INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Require C++23
target_compile_features(cpp_core INTERFACE cxx_std_23)

# Enable C++23 module support
set(CMAKE_CXX_MODULE_STD 23)
set(CMAKE_CXX_MODULE_EXTENSIONS OFF)

# Install rules --------------------------------------------------------------
include(GNUInstallDirs)

install(TARGETS cpp_core
    EXPORT cpp_coreTargets)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# CMake package configuration ------------------------------------------------
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/cpp_coreConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

configure_package_config_file(
    ${CMAKE_CURRENT_LIST_DIR}/cmake/cpp_core_config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cpp_coreConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cpp_core)

install(EXPORT cpp_coreTargets
    FILE cpp_coreTargets.cmake
    NAMESPACE cpp_core::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cpp_core)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/cpp_coreConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/cpp_coreConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cpp_core)

if(CMAKE_EXPORT_COMPILE_COMMANDS AND EXISTS "${CMAKE_BINARY_DIR}/compile_commands.json")
    add_custom_target(copy-compile-commands ALL
        ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/compile_commands.json
        DEPENDS ${CMAKE_BINARY_DIR}/compile_commands.json
        COMMENT "Copying compile_commands.json to project root"
    )
endif()
