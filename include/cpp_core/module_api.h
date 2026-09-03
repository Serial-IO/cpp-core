#pragma once

/**
 * Give an exported function C language linkage and platform-specific symbol visibility.
 * This macro is intended for C++ declarations and definitions only.
 */
#if defined(_WIN32) || defined(__CYGWIN__)
#ifdef cpp_bindings_windows_EXPORTS
#define MODULE_API extern "C" __declspec(dllexport)
#else
#define MODULE_API extern "C" __declspec(dllimport)
#endif
#else
#define MODULE_API extern "C" __attribute__((visibility("default")))
#endif
