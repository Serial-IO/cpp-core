cmake_minimum_required(VERSION 3.30)

# Export compile commands to root directory
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/CPM.cmake)

CPMAddPackage(
    NAME cmake-git-versioning
    GITHUB_REPOSITORY Katze719/cmake-git-versioning
    GIT_TAG v1.0.0
)

# Include the cmake-git-versioning module
include(${cmake-git-versioning_SOURCE_DIR}/cmake/cmake-git-versioning.cmake)

# Get git version info (sets CMake variables)
get_git_version_info()
generate_git_version(OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp_core)

project(
    cpp-core
    VERSION "${GIT_VERSION_MAJOR}.${GIT_VERSION_MINOR}.${GIT_VERSION_PATCH}"
    DESCRIPTION "Cross-platform helper library shared by cpp-linux-bindings and cpp-windows-bindings"
    LANGUAGES CXX
)

# Header-only library --------------------------------------------------------
add_library(cpp_core INTERFACE)
add_library(cpp_core::cpp_core ALIAS cpp_core)

# Public include directories
target_include_directories(
    cpp_core
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

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

install(
    TARGETS cpp_core
    EXPORT cpp_coreTargets
)

install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# CMake package configuration ------------------------------------------------
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/cpp_coreConfigVersion.cmake
    VERSION "${GIT_VERSION_MAJOR}.${GIT_VERSION_MINOR}.${GIT_VERSION_PATCH}"
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    ${CMAKE_CURRENT_LIST_DIR}/cmake/cpp_core_config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cpp_coreConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cpp_core
)

install(
    EXPORT cpp_coreTargets
    FILE cpp_coreTargets.cmake
    NAMESPACE cpp_core::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cpp_core
)

install(
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/cpp_coreConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/cpp_coreConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cpp_core
)

if(PROJECT_IS_TOP_LEVEL AND CMAKE_EXPORT_COMPILE_COMMANDS AND EXISTS "${CMAKE_BINARY_DIR}/compile_commands.json")
    add_custom_target(
        copy-compile-commands
        ALL
        ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/compile_commands.json
        DEPENDS ${CMAKE_BINARY_DIR}/compile_commands.json
        COMMENT "Copying compile_commands.json to project root"
    )
endif()
