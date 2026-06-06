if(NOT DEFINED GENERATOR)
    message(FATAL_ERROR "GENERATOR must point to the cpp_core_bindgen executable.")
endif()

execute_process(
    COMMAND "${GENERATOR}"
    RESULT_VARIABLE generator_result
    OUTPUT_VARIABLE generator_output
    ERROR_VARIABLE generator_error
)

if(NOT generator_result EQUAL 0)
    message(FATAL_ERROR "Generator failed with code ${generator_result}: ${generator_error}")
endif()

set(required_snippets
    "export const symbols = {"
    "serialOpen: {"
    "result: \"isize\""
    "serialClose: {"
    "result: \"i32\""
    "serialInBytesTotal: {"
    "result: \"i64\""
    "export const operations = {"
    "export class StatusCodeError extends Error"
    "export function createBindings(dylib: BindgenLibrary)"
    "serialOpen(args: SerialOpenParams): bigint"
)

foreach(snippet IN LISTS required_snippets)
    string(FIND "${generator_output}" "${snippet}" snippet_index)
    if(snippet_index EQUAL -1)
        message(FATAL_ERROR "Missing expected snippet in generated output: ${snippet}")
    endif()
endforeach()
