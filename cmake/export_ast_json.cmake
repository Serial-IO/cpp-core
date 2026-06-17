cmake_minimum_required(VERSION 3.30)

foreach(required_var
        CPP_CORE_AST_CLANGXX
        CPP_CORE_AST_INCLUDE_DIR
        CPP_CORE_AST_OUTPUT
        CPP_CORE_AST_SOURCE
        CPP_CORE_AST_STANDARD)
    if(NOT DEFINED ${required_var} OR "${${required_var}}" STREQUAL "")
        message(FATAL_ERROR "Missing required variable: ${required_var}")
    endif()
endforeach()

get_filename_component(_cpp_core_ast_output_dir "${CPP_CORE_AST_OUTPUT}" DIRECTORY)
file(MAKE_DIRECTORY "${_cpp_core_ast_output_dir}")

execute_process(
    COMMAND
        "${CMAKE_COMMAND}" -E env
        CCACHE_DISABLE=1
        "${CPP_CORE_AST_CLANGXX}"
        "-std=c++${CPP_CORE_AST_STANDARD}"
        "-I${CPP_CORE_AST_INCLUDE_DIR}"
        -fsyntax-only
        -Xclang
        -ast-dump=json
        "${CPP_CORE_AST_SOURCE}"
    OUTPUT_FILE "${CPP_CORE_AST_OUTPUT}"
    ERROR_VARIABLE _cpp_core_ast_stderr
    RESULT_VARIABLE _cpp_core_ast_result
)

if(NOT _cpp_core_ast_result EQUAL 0)
    file(REMOVE "${CPP_CORE_AST_OUTPUT}")
    string(STRIP "${_cpp_core_ast_stderr}" _cpp_core_ast_stderr)
    message(
        FATAL_ERROR
        "clang AST export failed with exit code ${_cpp_core_ast_result}.\n${_cpp_core_ast_stderr}"
    )
endif()

if(NOT EXISTS "${CPP_CORE_AST_OUTPUT}")
    message(FATAL_ERROR "clang AST export did not produce ${CPP_CORE_AST_OUTPUT}")
endif()

file(SIZE "${CPP_CORE_AST_OUTPUT}" _cpp_core_ast_size)
if(_cpp_core_ast_size EQUAL 0)
    file(REMOVE "${CPP_CORE_AST_OUTPUT}")
    message(FATAL_ERROR "clang AST export produced an empty file: ${CPP_CORE_AST_OUTPUT}")
endif()
