cmake_minimum_required(VERSION 3.30)

project(
    cpp-core
    VERSION 0.1.0
    DESCRIPTION "Core library with header interface files."
    LANGUAGES CXX
)

# ---------------------------------------------------------------------------
# Version information - can be overridden by parent project
# ---------------------------------------------------------------------------
foreach(_part IN ITEMS MAJOR MINOR PATCH)
    if(NOT DEFINED CPP_CORE_VERSION_${_part} OR CPP_CORE_VERSION_${_part} STREQUAL "")
        set(CPP_CORE_VERSION_${_part} ${PROJECT_VERSION_${_part}})
    endif()
endforeach()

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

# Require C++23
target_compile_features(cpp_core INTERFACE cxx_std_23)

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
    VERSION ${PROJECT_VERSION}
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
