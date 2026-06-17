cmake_minimum_required(VERSION 3.30)

# Export compile commands to root directory
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(cpp-core LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_MODULE_STD 26)
set(CMAKE_CXX_MODULE_EXTENSIONS OFF)

option(CPP_CORE_ENABLE_AST_EXPORT "Enable clang-based JSON AST export for the FFI headers" ON)
set(
    CPP_CORE_AST_JSON_OUTPUT
    "${CMAKE_BINARY_DIR}/ast/cpp_core_ffi_ast.json"
    CACHE FILEPATH
    "Output path for the generated FFI AST JSON file"
)
set(
    CPP_CORE_AST_CLANGXX
    ""
    CACHE FILEPATH
    "Path to the clang++ executable used for AST JSON export"
)
set(
    CPP_CORE_AST_SLIM_JSON_OUTPUT
    "${CMAKE_BINARY_DIR}/ast/cpp_core_ffi_api.json"
    CACHE FILEPATH
    "Output path for the reduced FFI API JSON metadata"
)

include(cmake/CPM.cmake)

CPMAddPackage(
    NAME cmake-git-versioning
    GITHUB_REPOSITORY Katze719/cmake-git-versioning
    GIT_TAG v1.1.0
)

# Include the cmake-git-versioning module
include(${cmake-git-versioning_SOURCE_DIR}/cmake/cmake-git-versioning.cmake)

# Get git version info (sets CMake variables)
get_git_version_info()
generate_git_version(OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/cpp_core)

set(PROJECT_VERSION "${GIT_VERSION_MAJOR}.${GIT_VERSION_MINOR}.${GIT_VERSION_PATCH}")
set(PROJECT_DESCRIPTION "Cross-platform helper library shared by cpp-linux-bindings and cpp-windows-bindings")

# Header-only library --------------------------------------------------------
add_library(cpp_core INTERFACE)
add_library(cpp_core::cpp_core ALIAS cpp_core)

add_library(cpp_core_strict_warnings INTERFACE)

target_compile_options(
    cpp_core_strict_warnings
    INTERFACE
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wsign-conversion
    -Wshadow
    -Wundef
    -Werror
)

# Public include directories
target_include_directories(
    cpp_core
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_options(cpp_core INTERFACE -freflection)

target_compile_features(cpp_core INTERFACE cxx_std_26)

include(CTest)

if(BUILD_TESTING)
    file(
        GLOB_RECURSE CPP_CORE_COMPILE_TEST_SOURCES
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/*.test.cpp"
    )
    add_library(cpp_core_compile_tests OBJECT ${CPP_CORE_COMPILE_TEST_SOURCES})
    target_link_libraries(cpp_core_compile_tests PRIVATE cpp_core::cpp_core)
    target_link_libraries(cpp_core_compile_tests PRIVATE cpp_core_strict_warnings)
endif()

if(CPP_CORE_ENABLE_AST_EXPORT)
    find_package(Python3 COMPONENTS Interpreter REQUIRED)

    file(
        GLOB _cpp_core_ast_interface_headers
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/cpp_core/interface/*.h"
    )

    set(
        _cpp_core_ast_headers
        "${CMAKE_CURRENT_SOURCE_DIR}/include/cpp_core/serial.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/cpp_core/error_callback.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/cpp_core/module_api.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/cpp_core/version.hpp"
        ${_cpp_core_ast_interface_headers}
    )

    if(CPP_CORE_AST_CLANGXX)
        set(_cpp_core_ast_clangxx "${CPP_CORE_AST_CLANGXX}")
    else()
        find_program(_cpp_core_ast_clangxx NAMES clang++)
    endif()

    if(NOT _cpp_core_ast_clangxx)
        message(
            FATAL_ERROR
            "CPP_CORE_ENABLE_AST_EXPORT=ON requires clang++. "
            "Install clang++ or set CPP_CORE_AST_CLANGXX to an explicit path."
        )
    endif()

    set(_cpp_core_ast_wrapper "${CMAKE_CURRENT_BINARY_DIR}/ast/cpp_core_ffi_ast.cpp")
    get_filename_component(_cpp_core_ast_json_dir "${CPP_CORE_AST_JSON_OUTPUT}" DIRECTORY)
    get_filename_component(_cpp_core_ast_slim_json_dir "${CPP_CORE_AST_SLIM_JSON_OUTPUT}" DIRECTORY)

    file(
        GENERATE
        OUTPUT "${_cpp_core_ast_wrapper}"
        CONTENT "#include <cpp_core/serial.h>\n"
    )

    add_custom_command(
        OUTPUT "${CPP_CORE_AST_JSON_OUTPUT}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${_cpp_core_ast_json_dir}"
        COMMAND
            ${CMAKE_COMMAND}
            -DCPP_CORE_AST_CLANGXX=${_cpp_core_ast_clangxx}
            -DCPP_CORE_AST_INCLUDE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/include
            -DCPP_CORE_AST_OUTPUT=${CPP_CORE_AST_JSON_OUTPUT}
            -DCPP_CORE_AST_SOURCE=${_cpp_core_ast_wrapper}
            -DCPP_CORE_AST_STANDARD=${CMAKE_CXX_STANDARD}
            -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/export_ast_json.cmake
        DEPENDS "${_cpp_core_ast_wrapper}" ${_cpp_core_ast_headers}
        COMMENT "Exporting FFI header AST JSON with clang++"
        VERBATIM
    )

    add_custom_command(
        OUTPUT "${CPP_CORE_AST_SLIM_JSON_OUTPUT}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${_cpp_core_ast_slim_json_dir}"
        COMMAND
            "${Python3_EXECUTABLE}"
            "${CMAKE_CURRENT_SOURCE_DIR}/tools/clang_ast_to_ffi_json.py"
            --input "${CPP_CORE_AST_JSON_OUTPUT}"
            --output "${CPP_CORE_AST_SLIM_JSON_OUTPUT}"
            --source-root "${CMAKE_CURRENT_SOURCE_DIR}"
            --public-header "cpp_core/serial.h"
        DEPENDS
            "${CPP_CORE_AST_JSON_OUTPUT}"
            "${CMAKE_CURRENT_SOURCE_DIR}/tools/clang_ast_to_ffi_json.py"
        COMMENT "Reducing clang AST JSON to compact FFI API metadata"
        VERBATIM
    )

    add_custom_target(cpp_core_ast_raw_json DEPENDS "${CPP_CORE_AST_JSON_OUTPUT}")
    add_custom_target(cpp_core_ast_slim_json DEPENDS "${CPP_CORE_AST_SLIM_JSON_OUTPUT}")
endif()

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

if(CPP_CORE_ENABLE_AST_EXPORT)
    install(
        FILES "${CPP_CORE_AST_SLIM_JSON_OUTPUT}"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cpp_core
        OPTIONAL
    )
endif()

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
