if(NOT DEFINED GENERATOR)
    message(FATAL_ERROR "GENERATOR must point to the cpp_core_bindgen executable.")
endif()

function(run_generator output_var)
    execute_process(
        COMMAND "${GENERATOR}" ${ARGN}
        RESULT_VARIABLE generator_result
        OUTPUT_VARIABLE generator_output
        ERROR_VARIABLE generator_error
    )

    if(NOT generator_result EQUAL 0)
        message(FATAL_ERROR "Generator failed with code ${generator_result}: ${generator_error}")
    endif()

    set(${output_var} "${generator_output}" PARENT_SCOPE)
endfunction()

function(require_snippets content label)
    foreach(snippet IN LISTS ARGN)
        string(FIND "${content}" "${snippet}" snippet_index)
        if(snippet_index EQUAL -1)
            message(FATAL_ERROR "Missing expected snippet in ${label}: ${snippet}")
        endif()
    endforeach()
endfunction()

function(forbid_snippets content label)
    foreach(snippet IN LISTS ARGN)
        string(FIND "${content}" "${snippet}" snippet_index)
        if(NOT snippet_index EQUAL -1)
            message(FATAL_ERROR "Unexpected snippet in ${label}: ${snippet}")
        endif()
    endforeach()
endfunction()

set(common_required
    "// Runtime profile: generic."
    "export type OpaquePointer = object;"
    "export type PointerValue = number | bigint | OpaquePointer;"
    "export interface BindgenRuntimeAdapter {"
    "export interface BindgenHost {"
    "pointerOf(value: ArrayBuffer | ArrayBufferView): PointerValue | null;"
    "export function createHostFromRuntimeAdapter(adapter: BindgenRuntimeAdapter): BindgenHost"
    "export function loadLibraryFromRuntimeAdapter("
    "createCallback<Definition extends NativeFunctionDefinition>("
    "export const symbols = {"
    "serialOpen: {"
    "result: \"isize\""
    "serialClose: {"
    "result: \"i32\""
    "serialInBytesTotal: {"
    "result: \"i64\""
    "export const operations = {"
    "export class StatusCodeError extends Error"
    "export const errorCallbackDefinition = {"
    "export type ErrorCallback = (error_code: number, message: string | null) => void;"
    "export function createErrorCallback(host: BindgenHost, callback: ErrorCallback): CallbackHandle"
    "export function createBindings(host: BindgenHost, dylib: BindgenLibrary)"
    "serialOpen(port: string | PointerValue, baudrate: number, data_bits: number, parity: number = 0, stop_bits: number = 0, error_callback: PointerValue | null = null): bigint"
)

set(generic_forbidden
    "export function createDenoHost(): BindgenHost"
    "export function loadDenoLibrary("
    "export async function createBunHost(): Promise<BindgenHost>"
    "export async function loadBunLibrary("
)

run_generator(generic_output)
require_snippets("${generic_output}" "generic output" ${common_required})
forbid_snippets("${generic_output}" "generic output" ${generic_forbidden})

set(deno_required
    "// Runtime profile: deno."
    "export function createDenoHost(): BindgenHost"
    "export function loadDenoLibrary("
)

set(deno_forbidden
    "export async function createBunHost(): Promise<BindgenHost>"
    "export async function loadBunLibrary("
)

run_generator(deno_output --runtime deno)
require_snippets("${deno_output}" "deno output" ${deno_required})
forbid_snippets("${deno_output}" "deno output" ${deno_forbidden})

set(bun_required
    "// Runtime profile: bun."
    "export async function createBunHost(): Promise<BindgenHost>"
    "export async function loadBunLibrary("
)

set(bun_forbidden
    "export function createDenoHost(): BindgenHost"
    "export function loadDenoLibrary("
)

run_generator(bun_output --runtime bun)
require_snippets("${bun_output}" "bun output" ${bun_required})
forbid_snippets("${bun_output}" "bun output" ${bun_forbidden})
