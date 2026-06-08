cmake_minimum_required(VERSION 3.30)

# Export compile commands to root directory
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(cpp-core LANGUAGES CXX)

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(FATAL_ERROR "cpp-core requires GCC 16+ with -std=c++26 and -freflection.")
endif()

if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 16)
    message(FATAL_ERROR "cpp-core requires GCC 16 or newer.")
endif()

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_MODULE_STD 26)
set(CMAKE_CXX_MODULE_EXTENSIONS OFF)

include(CheckCXXCompilerFlag)
include(CheckCXXSourceCompiles)

check_cxx_compiler_flag("-freflection" CPP_CORE_HAS_FREFLECTION_FLAG)
if(NOT CPP_CORE_HAS_FREFLECTION_FLAG)
    message(FATAL_ERROR "GCC 16 was found, but -freflection is not supported by this compiler build.")
endif()

set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -std=c++26 -freflection")
check_cxx_source_compiles(
    "
    #include <meta>
    #ifndef __cpp_impl_reflection
    #error Reflection feature macro missing
    #endif
    constexpr auto kReflectionProbe = ^^int;
    static_assert(kReflectionProbe == ^^int);
    int main() { return 0; }
    "
    CPP_CORE_HAS_WORKING_REFLECTION
)
unset(CMAKE_REQUIRED_FLAGS)

if(NOT CPP_CORE_HAS_WORKING_REFLECTION)
    message(FATAL_ERROR "Reflection support is required. Configure with GCC 16+ and a working -freflection implementation.")
endif()

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

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
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
endif()

# Public include directories
target_include_directories(
    cpp_core
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Require C++26 reflection on every consumer of the interface target.
target_compile_features(cpp_core INTERFACE cxx_std_26)
target_compile_options(cpp_core INTERFACE -freflection)

add_executable(cpp_core_bindgen EXCLUDE_FROM_ALL tools/generate_deno_symbols.cpp)
target_link_libraries(cpp_core_bindgen PRIVATE cpp_core::cpp_core)
target_link_libraries(cpp_core_bindgen PRIVATE cpp_core_strict_warnings)

include(CTest)

if(BUILD_TESTING)
    file(GLOB CPP_CORE_COMPILE_TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tests/*_compile_test.cpp")
    add_library(cpp_core_compile_tests OBJECT ${CPP_CORE_COMPILE_TEST_SOURCES})
    target_link_libraries(cpp_core_compile_tests PRIVATE cpp_core::cpp_core)
    target_link_libraries(cpp_core_compile_tests PRIVATE cpp_core_strict_warnings)

    set(CPP_CORE_DENO_BINDGEN_SMOKE_EXE "${CMAKE_CURRENT_BINARY_DIR}/cpp_core_bindgen_smoke_bin")
    add_custom_target(
        cpp_core_bindgen_smoke
        COMMAND
        ${CMAKE_COMMAND}
        -E
        copy
        $<TARGET_FILE:cpp_core_bindgen>
        ${CPP_CORE_DENO_BINDGEN_SMOKE_EXE}
        COMMAND
        ${CMAKE_COMMAND}
        -DGENERATOR=${CPP_CORE_DENO_BINDGEN_SMOKE_EXE}
        -P
        ${CMAKE_CURRENT_SOURCE_DIR}/tools/check_generated_deno_symbols.cmake
        DEPENDS cpp_core_bindgen
        COMMENT "Running smoke check for generated Deno bindgen output"
        VERBATIM
    )
endif()

# Install rules --------------------------------------------------------------
include(GNUInstallDirs)

install(
    TARGETS cpp_core
    EXPORT cpp_coreTargets
)

install(
    TARGETS cpp_core_bindgen
    EXPORT cpp_coreTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
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
